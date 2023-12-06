#include <bench/bench.h>
#include <utils/async-log.hpp>

#ifdef NDEBUG
#define DEBUG_SLEEP_FOR(...)
#define DEBUG_SLEEP_FOR_MS(...)
#else
#define DEBUG_SLEEP_FOR(t) std::this_thread::sleep_for(t)
#define DEBUG_SLEEP_FOR_MS(n) DEBUG_SLEEP_FOR(utils::Milliseconds{n});
#endif

namespace {

void run_default(std::string_view msg) { LOG_INFO(msg); }

void run_raw_cout(std::string_view msg) {
  utils::STDCoutGuard::RunWithGlobalLock([&] {
    std::cout.write(msg.data(), msg.size());
    std::cout.put(utils::CRLF);
  });
  std::cout.flush();
}

void run_raw_println(std::string_view msg) {
  utils::STDCoutGuard::Println(msg);
}

void run_async(utils::AsyncLogger *logger, std::string_view msg) {
  ASYNC_LOG_INFO(logger, msg);
}

template <typename F>
ALWAYS_INLINE utils::TimeCost::Clock::duration
RunWithCoutNull(F &&f, std::string_view label, size_t concurrency) {
#ifdef NDEBUG
  std::freopen("/dev/null", "w", stdout);
#endif

  auto &&new_label = fmt::format("{} concurrency={}", label, concurrency);
  std::list<std::thread> threads;
  SCOPE_EXIT({
    for (auto &&t : threads) {
      t.join();
    }
  });
  utils::Waiter ready_flag, waiter, finish;
  std::atomic_uint64_t tcnt = concurrency;
  std::atomic_uint64_t tcnt_ready = concurrency;
  rp(tid, concurrency) {
    threads.emplace_back([&, tid] {
      {
        if (--tcnt_ready == 0) {
          ready_flag.WakeOne();
        }
        waiter.Wait();
      }
      f();
      if (--tcnt == 0) {
        finish.WakeOne();
      }
    });
  }

  utils::TimeCost tc(new_label, false);
  {
    ready_flag.Wait();
    waiter.WakeAll();
    finish.Wait();
  }
  auto dur = tc.Duration();

  std::freopen("/dev/tty", "w", stdout);

  tc.Show(dur);

  return dur;
}

static void bench_async_log(int argc, char **argv) {
#ifdef NDEBUG
  const size_t n = 1e7;
  const size_t msg_size = 128;
#else
  const size_t n = 10;
  const size_t msg_size = 10;
#endif

  size_t concurrency = 1;

  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";
  std::string msg(msg_size, 't');
  if (argc > 1) {
    concurrency = std::stoi(argv[1]);
  }

  auto fn_show_rate = [&](utils::TimeCost::Clock::duration dur) {
    double rate = 1.0 * n * (msg_size + 58) / 1024 / 1024 /
                  utils::TimeCost::IntoDurationSec(dur) * concurrency;

    FMSGLN("    IO rate: {:.3f} MB/s", rate);
    bench::ShowDurAvgAndOps(dur, n * concurrency);
  };

  auto fn_bench_default = [&]() {
    auto dur = RunWithCoutNull(
        [&] {
          rp(i, n) {
            run_default(msg);
            DEBUG_SLEEP_FOR_MS(1);
          }
        },
        "default(sync, fmt::print)", concurrency);
    fn_show_rate(dur);
  };

  auto fn_bench_raw = [&]() {
    const auto *ptr = ALIGN_TO((const char *)("tests-string-message"), 4096);
    if (0) {
      auto dur = RunWithCoutNull(
          [&] {
            rp(i, n) { run_raw_cout({ptr, msg_size + 58}); }
          },
          "raw(std::cout)", concurrency);
      fn_show_rate(dur);
    }
    {
      auto dur = RunWithCoutNull(
          [&] {
            rp(i, n) { run_raw_println({ptr, msg_size + 58}); }
          },
          "raw(fmt::print)", concurrency);
      fn_show_rate(dur);
    }
  };

  auto fn_bench_async = [&]() {
    auto dur = RunWithCoutNull(
        [&] {
          auto *logger = &utils::AsyncLogger::GlobalStdout();
          rp(i, n) {
            run_async(logger, msg);
            DEBUG_SLEEP_FOR_MS(1);
          }
          logger->Flush();
          return n;
        },
        "async", concurrency);
    fn_show_rate(dur);
  };

  if (mode_str == "ALL") {
    fn_bench_default();
    fn_bench_async();
    fn_bench_raw();
  } else if (mode_str == "ASYNC") {
    fn_bench_async();
  } else if (mode_str == "DEFAULT") {
    fn_bench_default();
  } else if (mode_str == "RAW") {
    fn_bench_raw();
  } else {
    PANIC("Unknown mode", mode_str);
  }
}

} // namespace

FUNC_FACTORY_REGISTER("ASYNC_LOG", bench_async_log)
