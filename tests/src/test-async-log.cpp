#include "tests/tests.h"
#include "utils/async-log.hpp"

namespace tests {

struct AsyncLogNotifer : utils::AtomicNotifierWap {
  bool IsAwake() const { return notifier().IsAwake(); }
  bool Wake() const {
    if (!notifier().Wake()) {
      return false;
    }
    upper_notifier_.Wake();
    return true;
  }
  void BlockedWait() { return notifier().BlockedWait(); }
  AsyncLogNotifer(utils::AsyncNotifier &upper_notifier)
      : upper_notifier_(upper_notifier) {}

  utils::AsyncNotifier &upper_notifier_;
};

static void _test_async_log1() {
  std::stringstream ss;
  auto &&fn_write_ss = [&](const char *p, const size_t n) { ss.write(p, n); };
  auto global_notifer = utils::Notifier{};
  auto async_log_notifier = AsyncLogNotifer{global_notifer};
  utils::UniqAsyncLog<> logger{2, 4 + utils::kLogTimePointSize};
  {
    logger.Add("1", async_log_notifier);
    logger.Consume(1, 1, fn_write_ss);
    ASSERT_EQ(logger.Size(), 2 + utils::kLogTimePointSize);
    ASSERT_EQ(logger.RemainBuffeSize(), 2);
    logger.Flush(fn_write_ss);
    auto s = ss.str();
    ASSERT_EQ(std::string_view(s.data() + utils::kLogTimePointSize, 1), "1");
  }
  {
    logger.Add("2", async_log_notifier);
    logger.Consume(1, 1, fn_write_ss);
    ASSERT_EQ(logger.Size(), 2 + utils::kLogTimePointSize);
    ASSERT_EQ(logger.RemainBuffeSize(), 2);
  }
  {
    logger.Add("3", async_log_notifier);
    logger.Consume(1, 1, fn_write_ss);
    ASSERT_EQ(logger.Size(), 25);
    ASSERT_EQ(logger.RemainBuffeSize(), 4);
    logger.Flush(fn_write_ss);
    ASSERT_EQ((utils::kLogTimePointSize + 2) * 3, ss.str().size());
  }
  { ss.str(""); }
  {
    ASSERT(logger.NeedFlush(utils::Nanoseconds{1}));
    ASSERT(!logger.NeedFlush(utils::Seconds{1024}));
  }
  {
    async_log_notifier.Wake();
    async_log_notifier.BlockedWait();
    ASSERT(!async_log_notifier.IsAwake());
    logger.Add("x", async_log_notifier);
    async_log_notifier.BlockedWait();
    logger.Consume(1, 1, fn_write_ss);
    logger.Flush(fn_write_ss);
  }
  {
    ss.clear();
    ss.str("");
    auto ori_size = logger.FlushedSize();
    std::list<std::thread> async_runners;
    std::atomic_uint64_t join_cnt = 0;
    const size_t loop_cnt = 256;
    const size_t thread_cnt = 4;
    utils::Waiter waiter;
    rp(i, thread_cnt) async_runners.emplace_back([&, i, loop_cnt] {
      waiter.Wait();
      rp(j, loop_cnt) {
        auto &&s = fmt::format(("{:05d}:{:05d}"), i, j);
        logger.Add(std::move(s), async_log_notifier);
      }
      join_cnt++;
      global_notifer.Wake();
    });
    waiter.WakeAll();
    size_t expect_size =
        async_runners.size() * (11 + 1 + utils::kLogTimePointSize) * (loop_cnt);
    for (;;) {
      bool stop = join_cnt == async_runners.size();
      if (const auto status =
              global_notifer.BlockedWaitFor(utils::Milliseconds{5});
          status == utils::Notifier::Status::Normal) {
        if (async_log_notifier.IsAwake()) {
          async_log_notifier.BlockedWait();
          logger.Consume(std::numeric_limits<uint32_t>::max(), 2, fn_write_ss);
        }
      }
      if (logger.NeedFlush(utils::Milliseconds{50})) {
        logger.Flush(fn_write_ss);
      }
      if (stop)
        break;
    }
    for (auto &&t : async_runners)
      t.join();
    logger.Flush(fn_write_ss);
    auto flushed_size = logger.FlushedSize() - ori_size;
    auto res = ss.str();
    ASSERT_EQ(flushed_size, res.size());
    ASSERT_EQ(flushed_size, expect_size);
  }
}

static void _test_async_log2() {
  struct StringstreamWriter : std::stringstream {
    using Base = std::stringstream;
    using Base::Base;

    void Write(const char *p, const size_t n) { (*this).write(p, n); }
  };
  using AsyncLogger = utils::UniqAsyncLog<>;
  StringstreamWriter ss{};
  AsyncLogger logger{2, 4096};
  {
    ss.clear();
    ss.str("");

    auto waiter_task_pool = utils::Waiter::New();

    auto task_pool = std::make_shared<utils::TaskPoolWorker>();

    auto log_flush =
        utils::AsyncLogFlushWorker<AsyncLogger *, StringstreamWriter *>::New(
            &logger, &ss);

    utils::Timer timer;
    timer.AsyncRun();
    utils::AsyncSteadyTaskRunner steady_task_runner;
    steady_task_runner.AsyncRun();

    std::list<std::thread> async_runners;
    const size_t ori_size = logger.FlushedSize();
    std::atomic_uint64_t join_cnt = 0;
    utils::Waiter waiter;

    auto task_pool_worker = steady_task_runner.AddTask(task_pool);
    auto log_flush_worker = steady_task_runner.AddTask(log_flush);
    task_pool_worker.Put(
        [waiter_task_pool](bool stop) -> bool {
          ASSERT(!stop);
          waiter_task_pool->WakeAll();
          return true;
        },
        {});
    waiter_task_pool->Wait();

    constexpr size_t loop_cnt = 256;
    constexpr size_t async_runner_cnt = 4;

    rp(i, async_runner_cnt) async_runners.emplace_back([&, i, loop_cnt] {
      waiter.Wait();
      rp(j, loop_cnt) {
        auto &&s = fmt::format(("{:05d}:{:05d}"), i, j);
        logger.Add(std::move(s), *log_flush);
      }
      join_cnt++;
    });

    log_flush_worker.RegisterSteadyFlushLog(timer, utils::Milliseconds{1},
                                            utils::Milliseconds{4});

    waiter.WakeAll();
    for (auto &&t : async_runners)
      t.join();
    {
      timer.StopAndWait();
      steady_task_runner.StopAndWait();
    }
    auto flushed_size = logger.FlushedSize() - ori_size;
    auto res = ss.str();
    ASSERT_EQ(flushed_size, res.size());
    size_t expect_size =
        async_runners.size() * (11 + 1 + utils::kLogTimePointSize) * (loop_cnt);
    ASSERT_EQ(flushed_size, expect_size);
  }
}

static void _test_async_log() {
  _test_async_log1();
  _test_async_log2();
}

} // namespace tests

FUNC_FACTORY_REGISTER("async_log", tests::_test_async_log);
