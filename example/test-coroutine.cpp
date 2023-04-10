#include "utils/async-log.hpp"
#include "utils/coroutine-header.h"

#define LOG_ASYNC_MODE 0

#if LOG_ASYNC_MODE
static auto *logger = &utils::AsyncLogger::GlobalSTDOUT();
#define LOG(...) ASYNC_LOG_INFO(logger, __VA_ARGS__)
#else
#define LOG(...) LOG_INFO(__VA_ARGS__)
#endif

namespace example {

using TaskPoolWorker = utils::TaskPoolWorker<>;
using CoContext =
    utils::async::CoContext<utils::DelegateWheelTimer, TaskPoolWorker::Worker>;

struct TestPromise;
struct TestCoRunner : utils::async::CoRunner<CoContext, TestPromise> {
  using Base = utils::async::CoRunner<CoContext, TestPromise>;
  using Base::Base;

  ~TestCoRunner() { LOG(__PRETTY_FUNCTION__); }

  TestCoRunner(Handle handle) : Base(handle) { LOG("2.1 create task"); }
};

struct TestPromise : utils::async::CoPromise<CoContext, void> {
  TestPromise() { LOG("1. create promie object"); }
  TestCoRunner get_return_object() {
    LOG("2. create coroutine return object, and the coroutine is "
        "created now");
    return std_coro::coroutine_handle<TestPromise>::from_promise(*this);
  }
  std_coro::suspend_always initial_suspend() {
    LOG("3. do you want to susupend the current coroutine?");
    LOG("4. suspend because return std_coro::suspend_always");
    return {};
  }
  std_coro::suspend_always final_suspend() noexcept {
    LOG("13. coroutine body finished, do you want to susupend the "
        "current coroutine?");
    LOG("14. suspend because return std_coro::suspend_always");
    return {};
  }

  void return_void() {
    LOG("12. coroutine don't return value, so return_void is called");
  }

  void unhandled_exception() {}

  ~TestPromise() { LOG(__PRETTY_FUNCTION__); }
};

struct AsyncDelayTask {
  bool await_ready() {
    LOG("6. do you want to suspend current coroutine?");
    LOG("7. yes, suspend becase awaiter.await_ready() return false");
    return false;
  }
  template <typename Handle> void await_suspend(Handle handle) {
    LOG("8. execute awaiter.await_suspend()");

    handle.promise().co_context()->timer().Schedule(
        [c = resume_context_](bool) mutable {
          LOG("`AsyncDelayTask` run & finish");
          c->Notify();
          LOG("`AsyncDelayTask` notify");
        },
        duration_);
    handle.promise().co_context()->Suspend(
        handle, [c = resume_context_](const utils::AsyncNotifierPtr &notifer) {
          LOG("`AsyncDelayTask` try to poll once");
          auto res = c->Poll(notifer);
          LOG("`AsyncDelayTask` poll result is {}", res);
          return res;
        });

    LOG("9. schedule a delay task for {}", duration_);
  }

  void await_resume() { LOG("10. {}", __PRETTY_FUNCTION__); }

  AsyncDelayTask(utils::Milliseconds duration,
                 const utils::async::ResumeAblePtr &resume_context =
                     utils::async::ResumeAble::New())
      : duration_(duration), resume_context_(resume_context) {}

protected:
  utils::async::ResumeAblePtr resume_context_;
  const utils::Milliseconds duration_;
};

struct Task2;
struct Promise2;
struct Promise2 : utils::async::CoPromise<CoContext, int> {
  Task2 get_return_object();
};
struct Task2 : utils::async::CoRunner<CoContext, Promise2> {
  using CoRunner::CoRunner;

  Task2(CoRunner &&src) : CoRunner(std::move(src)) {}

  template <typename NextHandle> void await_suspend(NextHandle next_handle) {
    handle()
        .promise()
        .mut_co_context(next_handle.promise().co_context())
        ->Suspend(handle(), {});
  }

  bool await_ready() { return handle().done(); }

  int await_resume() {
    auto res = TakeValue();
    ASSERT(res);
    return *res;
  }
};

Task2 Promise2::get_return_object() {
  return std_coro::coroutine_handle<Promise2>::from_promise(*this);
}

Task2 suspend_none() {
  LOG("suspend_none");
  co_return 0;
}

Task2 suspend_one() {
  LOG("suspend_one \\");
  {
    auto &&dur = utils::Milliseconds{5};
    LOG("start to wait for {}", dur);
    co_await utils::async::AsyncSleepFor(dur);
    LOG("wait done");
  }
  LOG("suspend_one /");
  co_return 1;
}

Task2 suspend_two() {
  co_await suspend_none();
  auto a = co_await suspend_one();
  co_await suspend_none();
  auto b = co_await suspend_one();
  co_return a + b;
}

Task2 suspend_five() {
  auto a = co_await suspend_two();
  auto b = co_await suspend_two();
  co_return 1 + a + b;
}

Task2 run() {
  LOG("run");
  auto a = co_await suspend_five();
  auto b = co_await suspend_five();
  auto c = co_await suspend_five();
  co_return 5 + a + b + c;
}

} // namespace example

#ifndef NDEBUG

namespace tests {

static void _test_coroutine() {
  utils::AsyncSteadyTaskRunner steady_task_runner;
  auto worker = steady_task_runner.AddTask(std::make_shared<TaskPoolWorker>());
  CoContext context(utils::DelegateWheelTimer::GlobalDelegateInstance(),
                    worker);

  auto t = []() -> example::TestCoRunner {
    LOG("5. begin to execute coroutine body");
    utils::async::ResumeAblePtr waiter = utils::async::ResumeAble::New();
    co_await example::AsyncDelayTask{utils::Milliseconds{20}, waiter};
    LOG("11. coroutine resumed, continue execcute coroutine "
        "body now");
  }();
  LOG("4.1 init context");
  auto x = t.IntoWorker(context);
  utils::async::BlockOn(x);
}

static void _test_coroutine2() {
  utils::AsyncSteadyTaskRunner steady_task_runner;
  auto worker = steady_task_runner.AddTask(std::make_shared<TaskPoolWorker>());
  CoContext context(utils::DelegateWheelTimer::GlobalDelegateInstance(),
                    worker);
  auto t = example::run().IntoWorker(context);
  utils::async::BlockOn(t);
  auto res = t.TakeValue();
  ASSERT(res.has_value());
  ASSERT_EQ(res.value(), 20);
}

} // namespace tests

int main() {
  tests::_test_coroutine();
  tests::_test_coroutine2();
}

#else

int main() { return 0; }

#endif
