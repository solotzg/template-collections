#pragma once

#include "utils/head_template.h"
#include "utils/spsc_queue.hpp"
#include "wheel-timer.hpp"

namespace utils {

struct Timer : AsyncThreadRunner {
  using Clock = SteadyClock;
  using TimePoint = Clock::time_point;
  using Delay = Milliseconds;
  struct Task;
  using TaskPtr = std::shared_ptr<Task>;
  using ProducerLock = MutexLockWrap;
  struct Queues;

  TimePoint Now() const { return SteadyClock::now(); }

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

  struct Queues {
    struct Node {
      bool operator<(const Node &tar) const {
        return time_point() > tar.time_point();
      }
      Node(const TimePoint &time_point, const TaskPtr &task)
          : time_point_(time_point), task_(task) {}
      const TimePoint &time_point() const { return time_point_; }

      TimePoint time_point_;
      TaskPtr task_;
    };
    using Queue = std::priority_queue<Node>;

    template <typename F> void InvokeTop(F &&f) const {
      rp(i, size()) {
        auto &lock = locks_[i];
        auto &queue = queues_[i];
        lock.RunWithMutexLock([&] {
          if (!queue.empty()) {
            f(queue.top());
          }
        });
      }
    }

    template <typename... Args> void Add(Args &&...args) {
      auto index = get_tid() & size_mask();
      locks_[index].RunWithMutexLock(
          [&] { queues_[index].emplace(std::forward<Args>(args)...); });
    }

    Queues(size_t n) : size_(NextPow2(n)), size_mask_(size_ - 1) {
      queues_.Allocate(size());
      locks_.Allocate(size());
      rp(i, size()) {
        queues_.New(i);
        locks_.New(i);
      }
    }

    ~Queues() {
      rp(i, size()) {
        queues_.Del(i);
        locks_.Del(i);
      }
      queues_.Deallocate(size());
      locks_.Deallocate(size());
    }

    bool Empty() const {
      rp(i, size()) {
        bool is_empty =
            locks_[i].RunWithMutexLock([&] { return queues_[i].empty(); });
        if (!is_empty)
          return false;
      }
      return true;
    }

    template <typename F> void Run(F &&f, const TimePoint &expire) {
      rp(i, size()) {
        for (;;) {
          auto &queue = queues_[i];
          std::optional<Node> data;
          locks_[i].RunWithMutexLock([&] {
            if (queue.empty())
              return;
            auto &top = queue.top();
            if (expire >= top.time_point()) {
              data = (std::move(top));
              queue.pop();
            }
          });
          if (data) {
            f(std::move(*data));
          } else {
            break;
          }
        }
      }
    }

  private:
    size_t size() const { return size_; }
    size_t size_mask() const { return size_mask_; }

  private:
    ConstSizeArray<Queue> queues_;
    ConstSizeArray<ProducerLock> locks_;
    const size_t size_;
    const size_t size_mask_;
  };

  TaskPtr Schedule(typename Task::Func &&task, const TimePoint &time_point) {
    auto res = std::make_shared<Task>(std::move(task));
    queues().Add(time_point, res);
    notifier_.Wake();
    return res;
  }

  TaskPtr Schedule(typename Task::Func &&task, const Delay &delay) {
    return Schedule(std::move(task), Clock::now() + delay);
  }

  void AsyncRun() {
    AsyncRunImpl([this]() { LoopRun(); }, "Timer");
  }

  void StopAndWait() {
    StopAndWaitImpl([&] { notifier_.Wake(); },
                    [&] { RunOneRound(TimePoint::max() /*always expire*/); });
  }

  Timer(bool run = false,
        size_t queue_cap = std::thread::hardware_concurrency())
      : queues_(queue_cap) {
    if (run)
      AsyncRun();
  }

  ~Timer() { StopAndWait(); }

  static Timer &GlobalInstance() {
    static Timer timer(true);
    return timer;
  }

private:
  Queues &queues() { return *queues_; }
  const Queues &queues() const { return *queues_; }

  TimePoint GetNextTimePoint() const {
    TimePoint tp = TimePoint::max();
    queues().InvokeTop(
        [&](const Queues::Node &ele) { tp = std::min(tp, ele.time_point()); });
    return tp;
  }

  void LoopRun() {
    while (!IsStop()) {
      const auto next_time_point = GetNextTimePoint();
      if (const auto now = Clock::now(); now >= next_time_point) {
        RunOneRound(now);
      } else {
        notifier_.BlockedWaitUtil(next_time_point);
      }
    }
    ASSERT(IsStop());
    RunOneRound(TimePoint::max() /*always expire*/);
    ASSERT(queues().Empty());
  }

protected:
  size_t RunOneRound(const TimePoint &expire = TimePoint::max()) {
    size_t task_cnt = 0;
    queues().Run(
        [&](Queues::Node &&node) {
          node.task_->Run(IsStop());
          ++task_cnt;
        },
        expire);
    return task_cnt;
  }

private:
  // cache unfriendly
  Notifier notifier_;
  AlignedStruct<Queues, BasicConfig::CPU_CACHE_LINE_SIZE> queues_;
};

namespace chrono {

struct Clock {
  SystemClock::time_point system_time_point() const {
    return data_->system.load(std::memory_order_acquire);
  }

  SteadyClock::time_point steady_time_point() const {
    return data_->steady.load(std::memory_order_acquire);
  }

  uint32_t steady_time_point_millisec() const {
    return data_->steady_time_point_millisec.load(std::memory_order_acquire);
  }

  Clock() { Update(); }

  void Update() {
    const auto steady_time_point = SteadyClock::now();
    const auto sys_time_point = SystemClock::now();
    data_->system = sys_time_point;
    data_->steady_time_point_millisec =
        duration_cast<Milliseconds>(steady_time_point.time_since_epoch())
            .count();
    data_->steady = steady_time_point;
  }

  struct Data {
    std::atomic<SystemClock::time_point> system{};
    std::atomic<SteadyClock::time_point> steady{};
    std::atomic<uint32_t> steady_time_point_millisec{};
  };
  AlignedStruct<Data, BasicConfig::CPU_CACHE_LINE_SIZE> data_;
};

struct AsyncClock : AsyncThreadRunner, MutexLockWrap {
  using Task = std::function<void(void)>;

  bool AsyncRun() {
    return AsyncRunImpl(
        [this] {
          for (; !IsStop();) {
            auto now = SteadyClock::now();
            RunOneRound();
            notifier()->BlockedWaitUtil(now + duration_);
          }
        },
        "AsyncClock");
  }

  bool StopAndWait() {
    return StopAndWaitImpl([this] { notifier()->Wake(); }, [] {});
    ClearTask();
  }

  void AddTask(Task &&task) {
    RunWithMutexLock([&] { tasks_.emplace_back(std::move(task)); });
  }

  void ClearTask() {
    RunWithMutexLock([&] { tasks_.clear(); });
  }

  AsyncClock(Milliseconds duration = Milliseconds{1}, bool run = false)
      : duration_(duration) {
    notifier_ = Notifier::New();
    if (run)
      AsyncRun();
  }

  ~AsyncClock() { StopAndWait(); }

  const Clock &clock() const { return clock_; }

  static AsyncClock &GlobalInstance() {
    static AsyncClock time(Milliseconds{1}, true);
    return time;
  }

  Milliseconds duration() const { return duration_; }

  const NotifierPtr &notifier() const { return notifier_; }

protected:
  void RunOneRound() {
    clock_.Update();
    RunTasks();
  }

  void RunTasks() {
    RunWithMutexLock([&] {
      for (auto &&task : tasks_) {
        (task)();
      }
    });
  }

protected:
  const Milliseconds duration_;
  NotifierPtr notifier_;
  std::list<Task> tasks_;
  Clock clock_;
};

struct AsyncSteadyClock {
  using time_point = SteadyClock::time_point;
  time_point now() const { return clock_.steady_time_point(); }
  const Clock &clock_;
};

struct AsyncSystemClock {
  using time_point = SystemClock::time_point;
  time_point now() const { return clock_.system_time_point(); }
  const Clock &clock_;
};

} // namespace chrono

struct WheelTimerBase {
  using TimePoint = uint32_t;
  using Delay = Milliseconds;

  WheelTimerBase(chrono::AsyncClock &async_clock,
                 size_t queue_cnt = std::thread::hardware_concurrency(),
                 size_t queue_cap = 8192)
      : clock_(async_clock.clock()), queues_(queue_cnt, queue_cap),
        wheel_timer_manager_(Milliseconds(async_clock.duration()).count(),
                             Now()) {}

  struct Task {
    TimePoint millisec_time_point;
    wheel_timer::TaskPtr task;
  };

  struct Queues {
    template <typename... Args> void Add(Args &&...args) {
      auto index = get_tid() & size_mask();
      locks_[index].RunWithMutexLock([&] {
        for (;;) {
          auto ok =
              queues_[index].GenProducer().Put(std::forward<Args>(args)...);
          if (ok)
            break;
          std::this_thread::yield();
        }
      });
    }

    Queues(size_t n, size_t cap) : size_(NextPow2(n)), size_mask_(size_ - 1) {
      queues_.Allocate(size());
      locks_.Allocate(size());
      rp(i, size()) {
        queues_.New(i, cap);
        locks_.New(i);
      }
    }

    ~Queues() {
      rp(i, size()) {
        queues_.Del(i);
        locks_.Del(i);
      }
      queues_.Deallocate(size());
      locks_.Deallocate(size());
    }

    template <typename F> void Run(F &&f) {
      rp(i, size()) {
        auto &queue = queues_[i];
        for (;;) {
          auto data = queue.GenCustomer().Get();
          if (!data)
            break;
          f(std::move(*data));
        }
      }
    }

  private:
    size_t size() const { return size_; }
    size_t size_mask() const { return size_mask_; }

  private:
    ConstSizeArray<SPSCQueue<Task>> queues_;
    ConstSizeArray<MutexLockWrap> locks_;
    const size_t size_;
    const size_t size_mask_;
  };

  TimePoint Now() const { return clock_.steady_time_point_millisec(); }

  ~WheelTimerBase() { Cleanup(); }

protected:
  wheel_timer::TaskPtr ScheduleImpl(wheel_timer::Task::Func &&f,
                                    const Delay &delay) {
    return ScheduleImpl(std::move(f), delay.count());
  }
  wheel_timer::TaskPtr ScheduleImpl(wheel_timer::Task::Func &&f,
                                    const uint32_t delay_millisec) {
    auto task = std::make_shared<wheel_timer::Task>(std::move(f));
    queues_->Add(Task{Now() + delay_millisec, task});
    return task;
  }

  void RunOneRound() {
    queues_->Run([&](auto &&node) {
      wheel_timer_manager_.AddTask(std::move(node.task),
                                   node.millisec_time_point);
    });
    wheel_timer_manager_.Tick(Now());
  }

  void Cleanup() {
    queues_->Run([&](auto &&node) { node.task->Run(true); });
  }

protected:
  const chrono::Clock &clock_;
  AlignedStruct<Queues, BasicConfig::CPU_CACHE_LINE_SIZE> queues_;
  wheel_timer::TimerManager wheel_timer_manager_;
};

struct WheelTimerDelegate : utils::NotifierPtr {};
struct WheelTimerLocalNotifier : utils::AtomicNotifierPtr {};

template <typename T> struct WheelTimer {};

template <> struct WheelTimer<WheelTimerDelegate> : WheelTimerBase {
  using Self = WheelTimer<WheelTimerDelegate>;

  template <typename... Args>
  WheelTimer(chrono::AsyncClock &async_time, Args &&...args)
      : WheelTimerBase(async_time, std::forward<Args>(args)...),
        notifier_{async_time.notifier()} {}

  template <typename... Args>
  static auto DelegateRun(chrono::AsyncClock &async_time, Args &&...args) {
    auto timer =
        std::make_shared<Self>(async_time, std::forward<Args>(args)...);
    async_time.AddTask([timer = timer] { timer->RunOneRound(); });
    return timer;
  }

  template <typename... Args> auto Schedule(Args &&...args) {
    auto res = WheelTimerBase::ScheduleImpl(std::forward<Args>(args)...);
    notifier_->Wake();
    return res;
  }

#ifndef NDEBUG
  static Self &GlobalDelegateInstance() {
    static auto timer =
        DelegateRun(chrono::AsyncClock::GlobalInstance(), 4, 512);
    return *timer;
  }
#endif

private:
  WheelTimerDelegate notifier_;
};

using DelegateWheelTimer = WheelTimer<WheelTimerDelegate>;

template <> struct WheelTimer<WheelTimerLocalNotifier> : WheelTimerBase {
  template <typename... Args>
  WheelTimer(chrono::AsyncClock &async_time, Args &&...args)
      : WheelTimerBase(async_time, std::forward<Args>(args)...),
        notifier_{utils::AtomicNotifier::New()} {}

  template <typename... Args> auto Schedule(Args &&...args) {
    auto res = WheelTimerBase::ScheduleImpl(std::forward<Args>(args)...);
    notifier_->Wake();
    return res;
  }

protected:
  WheelTimerLocalNotifier notifier_;
};

struct WheelTimerRunner : WheelTimer<WheelTimerLocalNotifier>,
                          AsyncThreadRunner {
  using Self = WheelTimerRunner;

  template <typename... Args>
  WheelTimerRunner(chrono::AsyncClock &async_time, bool async_run,
                   Args &&...args)
      : WheelTimer(async_time, std::forward<Args>(args)...) {
    if (async_run) {
      async_time.AddTask([notifer = notifier_]() { notifer->Wake(); });
      AsyncRunImpl([this] { LoopRun(); }, "WheelTimer");
    }
  }

  void LoopRun() {
    while (!IsStop()) {
      RunOneRound();
      notifier_->BlockedWait();
    }
    ASSERT(IsStop());
  }

  ~WheelTimerRunner() { StopAndWait(); }

  bool StopAndWait() {
    return StopAndWaitImpl([this] { notifier_->Wake(); },
                           [this] { Cleanup(); });
  }

  static Self &GlobalInstance() {
    static auto timer = Self(chrono::AsyncClock::GlobalInstance(), true);
    return timer;
  }
};

} // namespace utils
