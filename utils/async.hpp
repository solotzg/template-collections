#pragma once

#include "utils/coroutine-header.h"
#include "utils/head_template.h"
#include "utils/task-pool.hpp"
#include "utils/timer.hpp"

namespace utils {

namespace async {

using CoroutineHandle = std_coro::coroutine_handle<>;

struct CoExecutorBase : utils::noncopyable {
  using PollResumeAble = std::function<bool(const utils::AsyncNotifierPtr &)>;

  void Push(CoroutineHandle handle, PollResumeAble &&resume_able = {}) {
    next_handle_ = handle;
    resume_able_ = std::move(resume_able);
  }

  bool Poll(const utils::AsyncNotifierPtr &notifer) {
    for (;;) {
      if (resume_able_) {
        if (!resume_able_(notifer)) {
          return false;
        } else {
          resume_able_ = {};
        }
      }
      auto handle = next_handle_;
      next_handle_ = {};
      if (!handle || handle.done()) {
        return true;
      }
      handle.resume();
    }
    return true;
  }

protected:
  CoroutineHandle next_handle_{};
  PollResumeAble resume_able_{};
};

template <typename Timer, typename AsyncTaskPool>
struct CoExecutor : CoExecutorBase {
  CoExecutor(Timer &timer, AsyncTaskPool &task_pool)
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
    handle.promise().co_executor()->timer().Schedule(
        [c = resume_context_](bool) mutable { c->Notify(); }, duration_);
    handle.promise().co_executor()->Push(
        handle, [c = resume_context_](const utils::AsyncNotifierPtr &notifer) {
          return c->Poll(notifer);
        });
  }

  constexpr void await_resume() {}
  bool await_ready() { return duration_ == utils::Milliseconds{}; }
  AsyncSleepFor(utils::Milliseconds duration,
                const ResumeAblePtr &resume_context = ResumeAble::New())
      : duration_(duration), resume_context_(resume_context) {}

protected:
  ResumeAblePtr resume_context_;
  const utils::Milliseconds duration_;
};

template <typename Task> auto BlockOn(Task &task) {
  thread_local auto local_notifer = utils::AtomicNotifier::New();
  for (;;) {
    if (auto res = task.Poll(local_notifer); bool(res))
      return res;
    local_notifer->BlockedWait();
  }
}

struct SuspendToExecutor {
  constexpr bool await_ready() const noexcept { return false; }
  template <typename HandleType>
  auto await_suspend(HandleType handle) const noexcept {
    handle.promise().co_executor()->Push(handle);
  }
  constexpr void await_resume() const noexcept {}
};

struct SuspendToNext {
  constexpr bool await_ready() const noexcept { return false; }
  template <typename HandleType>
  auto await_suspend(HandleType handle) const noexcept {
    ASSERT(handle.promise().next_handle());
    return handle.promise().next_handle();
  }
  constexpr void await_resume() const noexcept {}
};

template <typename Executor> struct PromiseExecutorWrap {
  Executor *co_executor() {
    ASSERT(co_executor_);
    return co_executor_;
  }
  Executor *&mut_co_executor(Executor *e) {
    co_executor_ = e;
    return co_executor_;
  }
  PromiseExecutorWrap(Executor &executor) : co_executor_(&executor) {}
  void unhandled_exception() { std::terminate(); }
  void rethrow_if_unhandled_exception() noexcept {}

protected:
  PromiseExecutorWrap() = default;

protected:
  Executor *co_executor_{};
};

template <typename Executor, bool await_able = true>
struct PromiseBase : PromiseExecutorWrap<Executor> {
  using Base = PromiseExecutorWrap<Executor>;
  using Base::Base;

  PromiseBase() = default;
  CoroutineHandle next_handle() { return next_handle_; }
  CoroutineHandle &mut_next_handle(CoroutineHandle handle) {
    next_handle_ = handle;
    return next_handle_;
  }
  auto final_suspend() noexcept { return SuspendToNext{}; }
  auto initial_suspend() noexcept { return std_coro::suspend_always{}; }

protected:
  CoroutineHandle next_handle_{};
};

template <typename Executor>
struct PromiseBase<Executor, false> : PromiseExecutorWrap<Executor> {
  using Base = PromiseExecutorWrap<Executor>;
  using Base::Base;

  PromiseBase(Executor &executor) : Base(executor) {}
  auto initial_suspend() noexcept { return SuspendToExecutor{}; }
  auto final_suspend() noexcept { return std_coro::suspend_always{}; }
};

template <typename Executor, typename Value, bool await_able>
struct PromiseOption : PromiseBase<Executor, await_able> {
  using Base = PromiseBase<Executor, await_able>;
  using ValueType = std::optional<Value>;
  using Base::Base;

  ValueType TakeValue() { return std::move(value_); }

protected:
  ValueType value_{};
};

template <typename Executor, typename Value, bool await_able = true>
struct CoPromise : PromiseOption<Executor, Value, await_able> {
  using Base = PromiseOption<Executor, Value, await_able>;
  using Base::Base;

  template <typename T> auto return_value(T &&t) {
    this->value_.emplace(std::move(t));
  }
};

template <typename Executor, bool await_able>
struct CoPromise<Executor, void, await_able>
    : PromiseBase<Executor, await_able> {
  using Base = PromiseBase<Executor, await_able>;
  using Base::Base;

  void return_void() noexcept {}
};

template <typename Executor, typename PromiseType> struct CoRunnerBase {
  using promise_type = PromiseType;
  using Handle = std_coro::coroutine_handle<PromiseType>;

  CoRunnerBase(Handle h) : handle_(h) {}
  CoRunnerBase(CoRunnerBase &&src) : handle_(src.handle_) {
    src.handle_ = nullptr;
  }
  ~CoRunnerBase() { DestroyHandle(); }

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

template <typename Context>
concept HasTakeValueMethod =
    requires {
      std::declval<typename Context::Handle &>().promise().TakeValue();
    };

template <typename Executor, typename PromiseType>
struct CoRunner : CoRunnerBase<Executor, PromiseType> {
  using Base = CoRunnerBase<Executor, PromiseType>;
  using Base::Base;

  auto TakeValue()
    requires HasTakeValueMethod<Base>
  {
    return Base::handle().promise().TakeValue();
  }
};

} // namespace async

} // namespace utils

#ifndef NDEBUG

namespace tests {

using CoExecutor = utils::async::CoExecutor<utils::DelegateWheelTimer,
                                            utils::TaskPoolWorker::Worker>;

struct TestPromise : utils::async::CoPromise<CoExecutor, void, false> {
  using Base = utils::async::CoPromise<CoExecutor, void, false>;
  TestPromise(CoExecutor &co_executor) : Base(co_executor) {}
  auto final_suspend() noexcept { return std_coro::suspend_always{}; }
  auto get_return_object() {
    return std_coro::coroutine_handle<TestPromise>::from_promise(*this);
  }
};

struct TestCoRunner : utils::async::CoRunner<CoExecutor, TestPromise> {
  using Base = utils::async::CoRunner<CoExecutor, TestPromise>;
  using Base::Base;
};

static void _test_async_sleep() {
  auto time_cost = utils::TimeCost{"", false};
  utils::AsyncSteadyTaskRunner steady_task_runner(true);
  auto worker =
      steady_task_runner.AddTask(std::make_shared<utils::TaskPoolWorker>());
  CoExecutor executor(utils::DelegateWheelTimer::GlobalDelegateInstance(),
                      worker);

  auto t = [](CoExecutor &) -> TestCoRunner {
    co_await utils::async::AsyncSleepFor(utils::Milliseconds{1});
  }(executor);
  utils::async::BlockOn(executor);
  ASSERT_GT(time_cost.Duration().count(), 1);
}

} // namespace tests

#endif