#pragma once

#include "list-def.h"
#include "utils.h"

namespace utils::wheel_timer {

constexpr uint32_t WHEEL_BITS = 6;
constexpr uint32_t ROOT_WHEEL_BITS = 8;
constexpr uint32_t WHEEL_SIZE(1 << WHEEL_BITS);
constexpr uint32_t ROOT_WHEEL_SIZE(1 << ROOT_WHEEL_BITS);
constexpr uint32_t WHEEL_SIZE_MASK(WHEEL_SIZE - 1);
constexpr uint32_t ROOT_WHEEL_SIZE_MASK(ROOT_WHEEL_SIZE - 1);

struct Task : MutexLockWrap {
  using Func =
      std::function<void(bool)>; // if triggered because of Timer is stopping
  Task(Func &&task) : task_(std::move(task)) {}

  bool Run(bool stop) {
    return RunWithMutexLock([&] {
      if (over_)
        return false;
      task_(stop);
      over_ = true;
      return true;
    });
  }

  bool Cancel() {
    return RunWithMutexLock([&] {
      if (over_)
        return false;
      over_ = true;
      return true;
    });
  }

private:
  Func task_;
  bool over_{false};
};
using TaskPtr = std::shared_ptr<Task>;

struct TimerNode : noncopyable {

  TimerNode(TaskPtr t) : task_(std::move(t)) {
    inner_list_init(&head_);
    expires_ = 0;
  }

  ListHead head_{};
  TaskPtr task_{};
  uint32_t expires_{};
};

struct Timer {

  ~Timer() {
#define M(v) Destroy((v).data(), (v).size());
    M(tv1);
    M(tv2);
    M(tv3);
    M(tv4);
    M(tv5);
#undef M
  }

  Timer(uint32_t jiffies) {
    timer_jiffies = jiffies;

    for (int i = 0; i < ROOT_WHEEL_SIZE; i++) {
      inner_list_init(&tv1[i]);
    }

    for (int i = 0; i < WHEEL_SIZE; i++) {
      inner_list_init(&tv2[i]);
      inner_list_init(&tv3[i]);
      inner_list_init(&tv4[i]);
      inner_list_init(&tv5[i]);
    }
  }

  void Add(TaskPtr task, uint32_t expires) {
    auto *node = (TimerNode *)(fast_bin.Alloc());
    new (node) TimerNode(std::move(task));
    node->expires_ = expires;
    Add(node);
  }

  static uint32_t calc_timer_index(uint32_t x, uint32_t n) {
    return (((x) >> ((ROOT_WHEEL_BITS) + (n) * (WHEEL_BITS))) &
            (WHEEL_SIZE_MASK));
  }

  void Run(uint32_t jiffies) {
    while ((int32_t)(jiffies - timer_jiffies) >= 0) {
      ListHead queued;
      int root_index = timer_jiffies & ROOT_WHEEL_SIZE_MASK;
      inner_list_init(&queued);
      if (root_index == 0) {
        int index = calc_timer_index(timer_jiffies, 0);
        Cascade(tv2.data(), index);
        if (index == 0) {
          index = calc_timer_index(timer_jiffies, 1);
          Cascade(tv3.data(), index);
          if (index == 0) {
            index = calc_timer_index(timer_jiffies, 2);
            Cascade(tv4.data(), index);
            if (index == 0) {
              index = calc_timer_index(timer_jiffies, 3);
              Cascade(tv5.data(), index);
            }
          }
        }
      }
      timer_jiffies++;
      inner_list_splice_init(&tv1[root_index], &queued);
      while (!inner_list_is_empty(&queued)) {
        TimerNode *node;
        node = inner_list_entry(queued.next, TimerNode, head_);
        node->task_->Run(false);
        inner_list_del_init(&node->head_);
        node->~TimerNode();
        fast_bin.Dealloc(node);
      }
    }
  }

protected:
  void Add(TimerNode *node) {
#define UPPER_SIZE(N) (uint32_t(1) << ((ROOT_WHEEL_BITS) + (N) * (WHEEL_BITS)))

    uint32_t expires = node->expires_;
    uint32_t idx = expires - timer_jiffies;
    ListHead *vec = nullptr;

    if (idx < UPPER_SIZE(0)) {
      int i = expires & ROOT_WHEEL_SIZE_MASK;
      vec = &tv1[i];
    } else if (idx < UPPER_SIZE(1)) {
      int i = calc_timer_index(expires, 0);
      vec = &tv2[i];
    } else if (idx < UPPER_SIZE(2)) {
      int i = calc_timer_index(expires, 1);
      vec = &tv3[i];
    } else if (idx < UPPER_SIZE(3)) {
      int i = calc_timer_index(expires, 2);
      vec = &tv4[i];
    } else if ((int32_t)idx < 0) {
      vec = &tv1[timer_jiffies & ROOT_WHEEL_SIZE_MASK];
    } else {
      int i = calc_timer_index(expires, 3);
      vec = &tv5[i];
    }
    inner_list_add_tail(&node->head_, vec);

#undef UPPER_SIZE
  }

  void Cascade(ListHead *v, int index) {
    ListHead queued;
    inner_list_init(&queued);
    inner_list_splice_init(v + index, &queued);
    while (!inner_list_is_empty(&queued)) {
      TimerNode *node;
      node = inner_list_entry(queued.next, TimerNode, head_);
      inner_list_del_init(&node->head_);
      Add(node);
    }
  }
  void Destroy(ListHead *v, size_t size) {
    for (size_t j = 0; j < size; j++) {
      ListHead *root = v + j;
      while (!inner_list_is_empty(root)) {
        TimerNode *node = inner_list_entry(root->next, TimerNode, head_);
        inner_list_del_init(&node->head_);
        node->task_->Run(true);
        node->~TimerNode();
        fast_bin.Dealloc(node);
      }
    }
  }

private:
  uint32_t timer_jiffies;
  std::array<ListHead, ROOT_WHEEL_SIZE> tv1;
  std::array<ListHead, WHEEL_SIZE> tv2;
  std::array<ListHead, WHEEL_SIZE> tv3;
  std::array<ListHead, WHEEL_SIZE> tv4;
  std::array<ListHead, WHEEL_SIZE> tv5;
  utils::FastBin<TimerNode> fast_bin;
};

struct TimerManager {
  TimerManager(uint32_t interval, uint32_t millisec_time_point)
      : current_(millisec_time_point),
        interval_(std::max(uint32_t(1), interval)),
        jiffies_(millisec_time_point / interval_), timer_(jiffies_) {}

  void Tick(uint32_t millisec_time_point) {
    while ((int32_t)(millisec_time_point - current_) >= 0) {
      timer_.Run(jiffies_);
      jiffies_++;
      current_ += interval_;
    }
  }

  void AddTask(TaskPtr task, uint32_t millisec_time_point) {
    timer_.Add(std::move(task),
               (millisec_time_point + interval_ - 1) / interval_);
  }

private:
  uint32_t interval_;
  uint32_t current_;
  uint32_t jiffies_;
  Timer timer_;
};

} // namespace utils::wheel_timer