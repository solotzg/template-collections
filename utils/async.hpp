#pragma once

#include "utils/coroutine-header.h"
#include "utils/head_template.h"
#include "utils/task-pool.hpp"
#include "utils/timer.hpp"

namespace utils {

namespace async {

using CoroutineHandle = std_coro::coroutine_handle<>;

struct CoContextBase : utils::noncopyable {
  using FnResumeAble = std::function<bool(const utils::AsyncNotifierPtr &)>;
  struct Frame : utils::noncopyable {
    Frame(CoroutineHandle &&handle, FnResumeAble &&fn_resume_able)
        : handle_(std::move(handle)),
          fn_resume_able_(std::move(fn_resume_able)) {}
    CoroutineHandle handle() const { return handle_; }
    bool resume_able(const utils::AsyncNotifierPtr &notifer) const {
      return fn_resume_able_(notifer);
    }

  protected:
    CoroutineHandle handle_{};
    FnResumeAble fn_resume_able_{};
  };

  void Suspend(CoroutineHandle handle, FnResumeAble &&fn_resume_able) {
    ASSERT(!frame_);
    ASSERT(handle);

    if (fn_resume_able)
      frame_.emplace(std::move(handle), std::move(fn_resume_able));
    else
      handles_.emplace(std::move(handle));
  }

  bool Poll(const utils::AsyncNotifierPtr &notifer) {
    for (;;) {
      if (frame_) {
        auto cur_handle = frame_->handle();
        ASSERT(cur_handle);
        if (cur_handle.done()) {
          frame_.reset();
        } else {
          if (frame_->resume_able(notifer)) {
            frame_.reset();
            handles_.emplace(std::move(cur_handle));
          } else {
            return false;
          }
        }
      }

      if (!handles_.empty()) {
        auto cur_handle = handles_.top();
        ASSERT(cur_handle);
        if (cur_handle.done()) {
          handles_.pop();
        } else {
          cur_handle.resume();
        }
      } else {
        return true;
      }
    }
  }

protected:
  std::stack<CoroutineHandle> handles_{};
  std::optional<Frame> frame_{};
};

template <typename Timer, typename AsyncTaskPool>
struct CoContext : CoContextBase {
  CoContext(Timer &timer, AsyncTaskPool &task_pool)
      : timer_(timer), task_pool_(task_pool) {}
  Timer &timer() { return timer_; }
  AsyncTaskPool &task_pool() { return task_pool_; }

protected:
  Timer &timer_;
  AsyncTaskPool &task_pool_;
};

struct ResumeAble;
using ResumeAblePtr = std::shared_ptr<ResumeAble>;

struct ResumeAble : MutexLockWrap {
  using Self = ResumeAble;

  void Notify() {
    resume_able_ = true;
    Wake();
  }

  bool Poll(const AsyncNotifierPtr &notifer) {
    if (resume_able_) {
      return true;
    }
    {
      RunWithMutexLock([&] { notifer_ = std::move(notifer); });
    }
    // check again
    return resume_able_;
  }

  static ResumeAblePtr New() { return std::make_shared<Self>(); }

protected:
  void Wake() {
    auto notifer = LoadNotifer();
    if (notifer)
      notifer->Wake();
  }

  AsyncNotifierPtr LoadNotifer() const {
    AsyncNotifierPtr notifer{};
    RunWithMutexLock([&] { notifer = notifer_; });
    return notifer;
  }

protected:
  AsyncNotifierPtr notifer_{};
  std::atomic_bool resume_able_{};
};
using ResumeAblePtr = std::shared_ptr<ResumeAble>;

struct AsyncSleepFor {
  template <typename Handle> void await_suspend(Handle handle) {
    handle.promise().co_context()->timer().Schedule(
        [c = resume_context_](bool) mutable { c->Notify(); }, duration_);
    handle.promise().co_context()->Suspend(
        handle, [c = resume_context_](const utils::AsyncNotifierPtr &notifer) {
          return c->Poll(notifer);
        });
  }

  constexpr void await_resume() {}
  bool await_ready() { return duration_ == Milliseconds{}; }
  AsyncSleepFor(Milliseconds duration,
                const ResumeAblePtr &resume_context = ResumeAble::New())
      : duration_(duration), resume_context_(resume_context) {}

  ResumeAblePtr resume_context_;
  const Milliseconds duration_;
};

template <typename Task> auto BlockOn(Task &task) {
  thread_local auto local_notifer = utils::AtomicNotifier::New();
  for (;;) {
    if (auto res = task.Poll(local_notifer); res)
      return res;
    local_notifer->BlockedWait();
  }
}

struct None {};
static constexpr None NoneOpt{};

template <typename Context> struct PromiseBase {
  const Context *co_context() const { return co_context_; }
  Context *co_context() { return co_context_; }

  Context *mut_co_context(Context *co_context) {
    co_context_ = co_context;
    return co_context_;
  }

protected:
  Context *co_context_{};
};

template <typename Context, typename Value>
struct PromiseOption : PromiseBase<Context> {
  using ValueType = std::optional<Value>;

  ValueType TakeValue() { return std::move(value_); }

protected:
  ValueType value_{};
};

template <typename Context, typename Value>
struct CoPromise : PromiseOption<Context, Value> {
  auto initial_suspend() noexcept { return std_coro::suspend_always{}; }
  auto final_suspend() noexcept { return std_coro::suspend_always{}; }
  void unhandled_exception() { std::terminate(); }
  void rethrow_if_unhandled_exception() noexcept {}
  //
  template <typename T> auto return_value(T &&t) {
    this->value_.emplace(std::move(t));
  }
};

template <typename Context>
struct CoPromise<Context, void> : PromiseBase<Context> {
  using ValueType = bool;

  auto initial_suspend() noexcept { return std_coro::suspend_always{}; }
  auto final_suspend() noexcept { return std_coro::suspend_always{}; }
  void unhandled_exception() { std::terminate(); }
  void rethrow_if_unhandled_exception() noexcept {}
  constexpr ValueType TakeValue() const { return true; }

  //
  void return_void() noexcept {}
};

template <typename Context, typename PromiseType> struct CoRunner {
  using promise_type = PromiseType;
  using Handle = std_coro::coroutine_handle<promise_type>;

  struct Worker : CoRunner {
    Worker(CoRunner &&src, Context &context) : CoRunner(std::move(src)) {
      handle().promise().mut_co_context(&context)->Suspend(handle(), {});
    }

    bool Poll(const utils::AsyncNotifierPtr &notifer) {
      return handle().promise().co_context()->Poll(notifer);
    }
  };

  Worker IntoWorker(Context &context) {
    return Worker(std::move(*this), context);
  }

  auto TakeValue() { return handle().promise().TakeValue(); }

  CoRunner(Handle h) : handle_(h) {}
  CoRunner(CoRunner &&src) : handle_(src.handle_) { src.handle_ = nullptr; }
  ~CoRunner() { DestroyHandle(); }

protected:
  const Handle &handle() const { return handle_; }
  void DestroyHandle() {
    if (!handle_)
      return;
    handle_.destroy();
    handle_ = nullptr;
  }

private:
  Handle handle_{};
};

} // namespace async

} // namespace utils

#ifndef NDEBUG

namespace tests {

using TaskPoolWorker = utils::TaskPoolWorker<>;
using CoContext =
    utils::async::CoContext<utils::DelegateWheelTimer, TaskPoolWorker::Worker>;

struct TestPromise;
struct TestCoRunner : utils::async::CoRunner<CoContext, TestPromise> {
  using Base = utils::async::CoRunner<CoContext, TestPromise>;
  using Base::Base;
};
struct TestPromise : utils::async::CoPromise<CoContext, void> {
  TestCoRunner get_return_object() {
    return std_coro::coroutine_handle<TestPromise>::from_promise(*this);
  }
};

static TestCoRunner SleepFor(const utils::Milliseconds duration) {
  co_await utils::async::AsyncSleepFor(duration);
}

static void _test_async_sleep() {
  auto time_cost = utils::TimeCost{"", false};
  auto dur = utils::Milliseconds{1};
  auto &&steady_task_runner = utils::AsyncSteadyTaskRunner::GlobalInstance();
  auto worker = steady_task_runner.AddTask(std::make_shared<TaskPoolWorker>());
  CoContext context(utils::DelegateWheelTimer::GlobalDelegateInstance(),
                    worker);
  {
    auto t = SleepFor(utils::Milliseconds{1}).IntoWorker(context);
    utils::async::BlockOn(t);
  }
  ASSERT_GT(time_cost.Duration().count(), 1);
  {
    auto t = SleepFor(utils::Milliseconds{0}).IntoWorker(context);
    ASSERT(t.Poll(nullptr));
  }
}

} // namespace tests

#endif