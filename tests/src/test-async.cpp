
#include "tests/tests.h"
#include "utils/async.hpp"

namespace tests {

using CoExecutor = utils::async::CoExecutor<utils::DelegateWheelTimer,
                                            utils::TaskPoolWorker::Worker>;

struct TestPromise : utils::async::CoPromise<CoExecutor, void, false> {
  using Base = utils::async::CoPromise<CoExecutor, void, false>;
  TestPromise() {}
  auto final_suspend() noexcept { return std_coro::suspend_always{}; }
  auto get_return_object() {
    return std_coro::coroutine_handle<TestPromise>::from_promise(*this);
  }
};

struct TestCoRunner : utils::async::CoRunner<CoExecutor, TestPromise> {
  using Base = utils::async::CoRunner<CoExecutor, TestPromise>;
  using Base::Base;
};

static void _test_async() {
  auto time_cost = utils::TimeCost{"", false};
  utils::AsyncSteadyTaskRunner steady_task_runner(true);
  auto worker =
      steady_task_runner.AddTask(std::make_shared<utils::TaskPoolWorker>());
  CoExecutor executor(utils::DelegateWheelTimer::GlobalDelegateInstance(),
                      worker);

  auto t = []() -> TestCoRunner {
    co_await utils::async::AsyncSleepFor(utils::Milliseconds{1});
  }();
  t.ViaExecutor(&executor);

  utils::async::BlockOn(executor);
  ASSERT_GT(time_cost.Duration().count(), 1);
}

} // namespace tests

FUNC_FACTORY_REGISTER("async", tests::_test_async);
