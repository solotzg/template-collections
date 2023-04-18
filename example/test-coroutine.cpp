#include "utils/async-log.hpp"
#include "utils/coroutine-header.h"
#include <filesystem>
#include <fstream>

#define LOG_ASYNC_MODE 0

#if LOG_ASYNC_MODE
static auto *logger = &utils::AsyncLogger::GlobalSTDOUT();
#define LOG(...) ASYNC_LOG_INFO(logger, __VA_ARGS__)
#else
#define LOG(...) LOG_INFO(__VA_ARGS__)
#endif

namespace example {

using CoExecutor = utils::async::CoExecutor<utils::DelegateWheelTimer,
                                            utils::TaskPoolWorker::Worker>;

struct TestPromise;
struct TestCoRunner : utils::async::CoRunner<CoExecutor, TestPromise> {
  using Base = utils::async::CoRunner<CoExecutor, TestPromise>;
  using Base::Base;

  ~TestCoRunner() { LOG(__PRETTY_FUNCTION__); }

  TestCoRunner(Handle handle) : Base(handle) { LOG("2.1 create task"); }
  TestCoRunner(TestCoRunner &&) = default;
};

struct TestPromise : utils::async::CoPromise<CoExecutor, void, false> {
  using Base = utils::async::CoPromise<CoExecutor, void, false>;
  TestPromise(CoExecutor &c) : Base(c) { LOG("1. create promie object"); }
  auto get_return_object() {
    LOG("2. create coroutine return object, and the coroutine is "
        "created now");
    return std_coro::coroutine_handle<TestPromise>::from_promise(*this);
  }
  auto initial_suspend() {
    LOG("3. do you want to susupend the current coroutine?");
    LOG("4. suspend because return std_coro::suspend_always");
    return Base::initial_suspend();
  }
  auto final_suspend() noexcept {
    LOG("13. coroutine body finished, do you want to susupend the "
        "current coroutine?");
    LOG("14. suspend because return std_coro::suspend_always");
    return Base::final_suspend();
  }
  void return_void() {
    LOG("12. coroutine don't return value, so return_void is called");
  }
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

    handle.promise().co_executor()->timer().Schedule(
        [c = resume_context_](bool) mutable {
          LOG("`AsyncDelayTask` finish");
          c->Notify();
          LOG("`AsyncDelayTask` notify");
        },
        duration_);

    handle.promise().co_executor()->Push(
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

struct RootTaskPromise : utils::async::CoPromise<CoExecutor, void, false> {
  using Base = utils::async::CoPromise<CoExecutor, void, false>;
  using Base::Base;

  auto get_return_object() {
    return std_coro::coroutine_handle<RootTaskPromise>::from_promise(*this);
  }
};

struct RootTask : utils::async::CoRunner<CoExecutor, RootTaskPromise> {
  using Base = utils::async::CoRunner<CoExecutor, RootTaskPromise>;
  using Base::Base;
};

struct AwaitTaskPromise : utils::async::CoPromise<CoExecutor, int> {
  using Base = utils::async::CoPromise<CoExecutor, int>;
  using Base::Base;

  auto get_return_object() {
    return std_coro::coroutine_handle<AwaitTaskPromise>::from_promise(*this);
  }
};

struct AwaitTask : utils::async::CoRunner<CoExecutor, AwaitTaskPromise> {
  using Base = utils::async::CoRunner<CoExecutor, AwaitTaskPromise>;
  using Base::Base;

  template <typename NextHandle> void await_suspend(NextHandle next_handle) {
    auto *co_executor =
        handle().promise().mut_co_executor(next_handle.promise().co_executor());
    co_executor->Push(handle());
    handle().promise().mut_next_handle(next_handle);
  }
  constexpr bool await_ready() { return false; }
  int await_resume() {
    auto res = TakeValue();
    ASSERT(res);
    return *res;
  }
};

struct ReadFileTask {
  template <typename NextHandle> void await_suspend(NextHandle next_handle) {
    next_handle.promise().co_executor()->Push(
        next_handle,
        [c = resume_context_](const utils::AsyncNotifierPtr &notifer) {
          return c->Poll(notifer);
        });
    next_handle.promise().co_executor()->task_pool().Put(
        [file_path = std::string(file_path_), waker = resume_context_,
         res = res_](bool stop) -> bool {
          if (stop) {
            waker->Notify();
            return true;
          }
          {
            std::ofstream file(file_path);
            file << 123456789;
          }
          int data{};
          {
            std::ifstream file(file_path);
            file >> data;
          }
          {
            auto res = std::filesystem::remove(file_path);
            ASSERT(res);
          }
          ASSERT_EQ(data, 123456789);
          res->emplace(data);
          waker->Notify();
          return true;
        },
        utils::MaxDuration);
  }
  constexpr bool await_ready() { return false; }
  auto await_resume() { return std::move(*res_); }

  ReadFileTask(std::string_view file_path,
               const utils::async::ResumeAblePtr &resume_context =
                   utils::async::ResumeAble::New())
      : file_path_(file_path), resume_context_(resume_context) {
    res_ = std::make_shared<std::optional<int>>();
  }

protected:
  utils::async::ResumeAblePtr resume_context_;
  std::string_view file_path_;
  std::shared_ptr<std::optional<int>> res_;
};

AwaitTask suspend_none() {
  LOG("suspend_none");
  co_return 0;
}

AwaitTask suspend_one() {
  LOG("suspend_one \\");
  {
    const auto &&dur = utils::Milliseconds{5};
    LOG("start to wait for {}", dur);
    co_await utils::async::AsyncSleepFor(dur);
    LOG("wait done");
  }
  LOG("suspend_one /");
  co_return 1;
}

AwaitTask suspend_two() {
  co_await suspend_none();
  auto a = co_await suspend_one();
  co_await suspend_none();
  auto b = co_await suspend_one();
  co_return a + b;
}

AwaitTask suspend_five() {
  auto a = co_await suspend_two();
  auto b = co_await suspend_two();
  co_return 1 + a + b;
}

RootTask run(CoExecutor &) {
  LOG("run");
  auto a = co_await suspend_five();
  auto b = co_await suspend_five();
  auto c = co_await suspend_five();
  auto d = co_await ReadFileTask("/tmp/__test_read_file");
  ASSERT_EQ(a + b + c, 15);
}

} // namespace example

#ifndef NDEBUG

namespace tests {

static void _test_coroutine() {
  utils::AsyncSteadyTaskRunner steady_task_runner(true);
  auto worker =
      steady_task_runner.AddTask(std::make_shared<utils::TaskPoolWorker>());
  CoExecutor context(utils::DelegateWheelTimer::GlobalDelegateInstance(),
                     worker);

  auto t = [](CoExecutor &) -> example::TestCoRunner {
    LOG("5. begin to execute coroutine body");
    utils::async::ResumeAblePtr waiter = utils::async::ResumeAble::New();
    co_await example::AsyncDelayTask{utils::Milliseconds{20}, waiter};
    LOG("11. coroutine resumed, continue execcute coroutine "
        "body now");
  }(context);
  LOG("4.1 init context");
  utils::async::BlockOn(context);
}

static void _test_coroutine2() {
  utils::AsyncSteadyTaskRunner steady_task_runner(true);
  auto worker =
      steady_task_runner.AddTask(std::make_shared<utils::TaskPoolWorker>());
  CoExecutor e(utils::DelegateWheelTimer::GlobalDelegateInstance(), worker);

  auto t = example::run(e);
  utils::async::BlockOn(e);
}

} // namespace tests

int main() {
  tests::_test_coroutine();
  tests::_test_coroutine2();
}

#else

int main() { return 0; }

#endif
