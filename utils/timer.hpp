#pragma once

#include "utils.h"
#include <optional>
#include <queue>
#include <thread>

struct Timer {
  using Clock = std::chrono::steady_clock;
  using TimePoint = Clock::time_point;
  using Delay = std::chrono::milliseconds;

  struct Task : MutexLockWrap {
    using Func =
        std::function<void(bool)>; // if triggered because of Timer is stopping
    Task(Func &&task) : task_(std::move(task)) {}

    bool Run(bool stop) {
      auto _ = GenLockGuard();
      if (over_)
        return false;
      task_(stop);
      over_ = true;
      return true;
    }

    bool Cancel() {
      auto _ = GenLockGuard();
      if (over_)
        return false;
      over_ = true;
      return true;
    }

  private:
    Func task_;
    bool over_{false};
  };
  using TaskPtr = std::shared_ptr<Task>;
  struct Node {
    bool operator<(const Node &tar) const {
      return time_point() > tar.time_point();
    }
    Node(const Delay &delay, const TaskPtr &task)
        : time_point_(Clock::now() + delay), task_(task) {}
    const TimePoint &time_point() const { return time_point_; }

    TimePoint time_point_;
    TaskPtr task_;
  };

  struct Queue : MutexLockWrap {
    template <typename F> void InvokeTop(F &&f) const {
      auto _ = GenLockGuard();

      const Node *ele{};
      if (!queue_.empty()) {
        ele = &queue_.top();
      }
      f(ele);
    }

    template <typename... _Args> void Add(_Args &&...__args) {
      auto _ = GenLockGuard();
      queue_.emplace(std::forward<_Args>(__args)...);
    }

    bool Empty() const {
      auto _ = GenLockGuard();
      return queue_.empty();
    }

    template <typename F>
    void Run(F &&f, const TimePoint &expire = Clock::now()) {
      bool stop = false;
      while (!stop) {
        stop = true;
        std::optional<Node> data;
        {
          auto _ = GenLockGuard();
          if (queue_.empty())
            break;
          auto &top = queue_.top();
          if (expire >= top.time_point()) {
            data = (std::move(top));
            queue_.pop();
          }
        }
        if (data) {
          f(std::move(*data));
          stop = false;
        }
      }
    }

    //
    std::priority_queue<Node> queue_;
  };

  TaskPtr Schedule(typename Task::Func &&task, const Delay &delay) {
    auto res = std::make_shared<Task>(std::move(task));
    queue_.Add(delay, res);
    notifier_.Wake();
    return res;
  }

  TimePoint GetNextTimePoint() const {
    TimePoint tp;
    queue_.InvokeTop([&](const Node *ele) {
      if (ele) {
        tp = ele->time_point();
      } else
        tp = default_tick_ + Clock::now();
    });
    return tp;
  }

  void LoopRun() {
    while (!stop_.load(std::memory_order_relaxed)) {
      if (auto status = notifier_.BlockedWaitUtil(GetNextTimePoint());
          Notifier::Status::Timeout == status) {
        RunOneRound();
      }
    }
    assert(stop_);
    queue_.Run([&](Node &&node) { node.task_->Run(true /*stop task*/); },
               TimePoint::max() /*always expire*/);
    assert(queue_.Empty());
  }

  size_t RunOneRound() {
    size_t task_cnt = 0;
    queue_.Run([&](Node &&node) {
      node.task_->Run(stop_.load(std::memory_order_relaxed));
      ++task_cnt;
    });
    return task_cnt;
  }

  void AsyncRun() {
    stop_ = false;
    assert(!context_);
    context_ = std::make_unique<std::thread>([this]() { LoopRun(); });
  }

  void Stop() {
    stop_ = true;
    notifier_.Wake();
  }

  void StopAndWait() {
    Stop();
    if (context_) {
      context_->join();
      context_.reset();
    }
    LoopRun();
  }

  Timer(const Delay &default_tick = Delay{50}) : default_tick_(default_tick) {}

  ~Timer() { StopAndWait(); }

private:
  const Delay default_tick_;
  std::atomic_bool stop_{};
  std::unique_ptr<std::thread> context_;
  // cache unfriendly
  alignas(BasicConfig::CPU_CACHE_LINE_SIZE) Notifier notifier_;
  alignas(BasicConfig::CPU_CACHE_LINE_SIZE) Queue queue_;
};

#ifndef NDEBUG
namespace tests {
static void _test_timer() {
  {
    Timer timer;

    std::atomic_bool flag = false;
    std::atomic_bool cancel_test = false;

    Timer::Delay delay{50};
    timer.Schedule(
        [&](bool stop) {
          assert(!stop);
          flag = true;
        },
        delay);
    auto cancel_case = timer.Schedule(
        [&](bool stop) {
          assert(!stop);
          cancel_test = true;
        },
        delay);
    assert(flag == false);
    std::this_thread::sleep_for(delay);
    assert(flag == false);
    assert(cancel_case->Cancel());
    assert(!cancel_case->Cancel());
    assert(timer.RunOneRound() == 2);
    assert(!cancel_test);
    assert(flag == true);
  }
  {
    Timer timer;

    timer.AsyncRun();
    auto old = Timer::Clock::now();
    auto check = old;
    Notifier notifier;
    Timer::Delay delay(123);
    timer.Schedule(
        [&](bool stop) {
          check = Timer::Clock::now();
          notifier.Wake();
          assert(!stop);
        },
        delay);
    notifier.BlockedWaitFor(std::chrono::milliseconds(10000));
    assert((check - old) >= delay);
    timer.Schedule([](bool stop) { assert(stop); }, Timer::Delay(1000 * 3600));
    timer.StopAndWait();
    timer.Schedule([](bool stop) { assert(stop); }, Timer::Delay(1000 * 3600));
  }
  {
    Timer timer;

    timer.AsyncRun();
    struct Test : MutexLockWrap {
      void Add(int x) {
        auto _ = GenLockGuard();
        res.emplace_back(x);
      }
      std::vector<int> res;
    } tests;
    const int test_cnt = 10;
    std::atomic_int cnt = test_cnt;
    Notifier notifier;
    for (int i = 0; i < test_cnt; ++i) {
      timer.Schedule(
          [&, i](bool stop) {
            assert(!stop);
            tests.Add(i);
            if (--cnt == 0) {
              notifier.Wake();
            }
          },
          Timer::Delay(200 - i * test_cnt));
    }
    notifier.BlockedWaitFor(std::chrono::milliseconds{3600 * 1000});
    for (int i = 0; i < test_cnt; ++i) {
      assert(tests.res[i] == (test_cnt - 1 - i));
    }
    timer.Stop();
  }
}
} // namespace tests
#endif