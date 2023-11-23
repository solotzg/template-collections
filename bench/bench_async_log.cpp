#include "bench/bench.h"
#include "utils/async-log.hpp"

namespace {

NO_INLINE void run_stl(std::string_view msg) { LOG_INFO(msg); }
NO_INLINE void run_raw(std::string_view msg) {
  utils::STDCoutGuard::Print(msg);
}
NO_INLINE void run_async(utils::AsyncLogger *logger, std::string_view msg) {
  ASYNC_LOG_INFO(logger, msg);
}

template <typename F>
ALWAYS_INLINE utils::TimeCost::Clock::duration
RunWithCoutNull(F &&f, std::string_view label) {
  std::freopen("/dev/null", "w", stdout);

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
#else
  const size_t n = 100;
#endif

  const size_t msg_size = 128;

  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";
  std::string msg(msg_size, 't');

  auto fn_show_rate = [&](utils::TimeCost::Clock::duration dur) {
    double rate = 1.0 * n * (msg_size + 58) / 1024 / 1024 /
                  utils::TimeCost::IntoDurationSec(dur);

    FMSGLN("[IO rate: {:.3f} MB/s]", rate);
  };

  auto fn_bench_stl = [&]() {
    auto dur = RunWithCoutNull(
        [&] {
          rp(i, n) { run_stl(msg); }
        },
        "stl");
    fn_show_rate(dur);
  };

  auto fn_bench_raw = [&]() {
    auto *p = __builtin_return_address(0);
    auto *ptr = (char *)((size_t)(p) / 4096 * 4096);
    auto dur = RunWithCoutNull(
        [&] {
          rp(i, n) { run_raw({ptr, msg_size + 58}); }
        },
        "raw");
    fn_show_rate(dur);
  };

  auto fn_bench_async = [&]() {
    auto dur = RunWithCoutNull(
        [&] {
          auto *logger = &utils::AsyncLogger::GlobalSTDOUT();
          rp(i, n) { run_async(logger, msg); }
          logger->Flush(std::cout);
          return n;
        },
        "async");
    fn_show_rate(dur);
  };

  if (mode_str == "ALL") {
    fn_bench_stl();
    fn_bench_async();
    fn_bench_raw();
  } else if (mode_str == "ASYNC") {
    fn_bench_async();
  } else if (mode_str == "STL") {
    fn_bench_stl();
  } else if (mode_str == "RAW") {
    fn_bench_raw();
  } else {
    PANIC("Unknown mode", mode_str);
  }
}

} // namespace

FUNC_FACTORY_REGISTER("ASYNC_LOG", bench_async_log)
