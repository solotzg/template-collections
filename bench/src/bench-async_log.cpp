#include <bench/bench.h>
#include <utils/async-log.hpp>


namespace {

void run_default(std::string_view msg) { LOG_INFO(msg); }

void run_raw_cout(std::string_view msg) {
  utils::STDCoutGuard::RunWithLock([&] {
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
RunWithCoutNull(F &&f, std::string_view label) {
#ifdef NDEBUG
  std::freopen("/dev/null", "w", stdout);
#endif

  utils::TimeCost tc(label, false);
  f();
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

  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";
  std::string msg(msg_size, 't');

  auto fn_show_rate = [&](utils::TimeCost::Clock::duration dur) {
    double rate = 1.0 * n * (msg_size + 58) / 1024 / 1024 /
                  utils::TimeCost::IntoDurationSec(dur);

    FMSGLN("    IO rate: {:.3f} MB/s", rate);
    bench::ShowDurAvgAndOps(dur, n);
  };

  auto fn_bench_default = [&]() {
    auto dur = RunWithCoutNull(
        [&] {
          rp(i, n) {
            run_default(msg);
#ifndef NDEBUG
            std::this_thread::sleep_for(utils::Milliseconds{1});
#endif
          }
        },
        "default(sync, fmt::print)");
    fn_show_rate(dur);
  };

  auto fn_bench_raw = [&]() {
    const auto *ptr = ALIGN_TO((const char *)("tests-string-message"), 4096);
    {
      auto dur = RunWithCoutNull(
          [&] {
            rp(i, n) { run_raw_cout({ptr, msg_size + 58}); }
          },
          "raw(std::cout)");
      fn_show_rate(dur);
    }
    {
      auto dur = RunWithCoutNull(
          [&] {
            rp(i, n) { run_raw_println({ptr, msg_size + 58}); }
          },
          "raw(fmt::print)");
      fn_show_rate(dur);
    }
  };

  auto fn_bench_async = [&]() {
    auto dur = RunWithCoutNull(
        [&] {
          auto *logger = &utils::AsyncLogger::GlobalStdout();
          rp(i, n) {
            run_async(logger, msg);
#ifndef NDEBUG
            std::this_thread::sleep_for(utils::Milliseconds{1});
#endif
          }
          logger->Flush();
          return n;
        },
        "async");
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
