#pragma once

#include <utils/mpsc.hpp>

namespace utils {

/*
  SetAsyncNotifier -> IsAvailable -> Run; * -> Stop;
*/
struct SteadyTask {
  virtual void Run() = 0;
  virtual bool IsAvailable() = 0;
  virtual void Stop() = 0;
  virtual void SetAsyncNotifier(AsyncNotifierPtr) = 0;
};

struct SteadyTaskWithParentNotifer : SteadyTask {
protected:
  void WakeParent() {
    ASSERT(parent_notifier_);
    parent_notifier_->Wake();
  }
  void SetAsyncNotifier(AsyncNotifierPtr notifier) override {
    parent_notifier_ = notifier;
  }
  AsyncNotifierPtr parent_notifier_{};
};

struct TaskPoolWorker final : MutexLockWrap,
                              AtomicNotifierWap,
                              SteadyTaskWithParentNotifer,
                              std::enable_shared_from_this<TaskPoolWorker> {
  using NeedWakeProducer = bool;
  using IsStop = bool;
  using Task = std::function<NeedWakeProducer(IsStop)>;
  using MPSC = MPSCQueueWithNotifer<MPSCWorker<Task>>;

  TaskPoolWorker(size_t producer_cnt = 4, size_t producer_cap = 1024,
                 size_t consume_batch_size = 256)
      : task_pool_mpsc_queue_(producer_cnt, producer_cap),
        consume_batch_size_(consume_batch_size) {
    producer_lock_.Allocate(producer_cnt);
    rp(i, producer_size()) producer_lock_.New(i);

    ASSERT(producer_lock_);
  }

  ~TaskPoolWorker() {
    rp(i, producer_size()) producer_lock_.Del(i);
    producer_lock_.Deallocate(producer_size());
  }

  struct Worker {
    bool Put(Task &&t, const Milliseconds &timeout) {
      return inner_->Put(std::move(t), std::move(timeout));
    }
    Worker(std::shared_ptr<TaskPoolWorker> src) : inner_(src) {}

  private:
    std::shared_ptr<TaskPoolWorker> inner_;
  };

  Worker IntoWorker() { return Worker{this->shared_from_this()}; }
  size_t producer_size() const {
    return task_pool_mpsc_queue().producer_size();
  }

private:
  bool Put(Task &&t, const Milliseconds &timeout) {
    size_t id = utils::get_tid() % producer_size();
    return producer_lock(id).RunWithMutexLock([&] {
      bool res = task_pool_mpsc_queue().Put(std::move(t), id, timeout,
                                            [&] { Wake(); });
      Wake();
      return res;
    });
  }

  MPSC &task_pool_mpsc_queue() { return task_pool_mpsc_queue_; }
  const MPSC &task_pool_mpsc_queue() const { return task_pool_mpsc_queue_; }
  MutexLockWrap &producer_lock(size_t index) const {
    ASSERT(producer_lock_);
    return producer_lock_[index];
  }

  bool IsAvailable() override { return notifier().IsAwake(); }

  void Stop() override {
    RunWithMutexLock([&] { RunOneRound(true); });
  }

  void Run() override {
    RunWithMutexLock([&] {
      notifier().BlockedWait();
      RunOneRound(false);
    });
  }

  void RunOneRound(bool stop) {
    for (;;) {
      auto cnt = task_pool_mpsc_queue().TryGet(
          [&](auto &&f, size_t producer_index) {
            if (f(stop)) {
              task_pool_mpsc_queue().producer_notifier(producer_index).Wake();
            }
          },
          [&](size_t pos) {
            task_pool_mpsc_queue().producer_notifier(pos).Wake();
          },
          consume_batch_size_);
      if (cnt == 0)
        break;
      task_pool_mpsc_queue().WakeAllProducers();
    }
  }

  bool Wake() {
    if (!notifier().Wake()) {
      return false;
    }
    WakeParent();
    return true;
  }

private:
  mutable utils::ConstSizeArray<MutexLockWrap> producer_lock_;
  MPSC task_pool_mpsc_queue_;
  const size_t consume_batch_size_;
};

struct AsyncSteadyTaskRunner : AsyncThreadRunner {
  using SteadyTaskPtr = std::shared_ptr<SteadyTask>;

  void AsyncRun() {
    AsyncRunImpl(
        [this] {
          for (; !IsStop();) {
            notifier_->BlockedWait();
            tasks_.RunOneRound();
          }
          tasks_.Stop();
        },
        "SteadyTask");
  }

  void StopAndWait() {
    StopAndWaitImpl([&] { notifier_->Wake(); }, [&] { tasks_.Stop(); });
  }

  template <typename T> auto AddTask(std::shared_ptr<T> task) {
    SteadyTaskPtr t = task;
    t->SetAsyncNotifier(notifier_);
    tasks_.Add(std::move(t));
    return task->IntoWorker();
  }

  AsyncSteadyTaskRunner(bool run = false) {
    notifier_ = AtomicNotifier::New();
    if (run)
      AsyncRun();
  }
  ~AsyncSteadyTaskRunner() { StopAndWait(); }

  static AsyncSteadyTaskRunner &GlobalInstance() {
    static AsyncSteadyTaskRunner t(true);
    return t;
  }

  struct Tasks : MutexLockWrap {

    void Add(SteadyTaskPtr &&t) {
      RunWithMutexLock([&] { tasks().emplace_back(std::move(t)); });
    }

    void RunOneRound() {
      RunWithMutexLock([&] {
        for (bool over = false; !over;) {
          over = true;
          for (auto &&t : tasks()) {
            if (t->IsAvailable()) {
              t->Run();
              over = false;
            }
          }
        }
      });
    }

    void Stop() {
      RunWithMutexLock([&] {
        for (auto &&t : tasks()) {
          t->Stop();
        }
        tasks().clear();
      });
    }

  private:
    std::vector<SteadyTaskPtr> &tasks() { return inner_; }
    const std::vector<SteadyTaskPtr> &tasks() const { return inner_; }

    std::vector<SteadyTaskPtr> inner_;
  };

private:
  AtomicNotifierPtr notifier_;
  Tasks tasks_;
};

} // namespace utils