#include "bench/bench.h"
#include "utils/head_define.h"
#include "utils/timer.hpp"

namespace bench {

static void bench_async_time(const size_t cnt) {
  auto &async_time = utils::chrono::AsyncClock::GlobalInstance();
  std::atomic<utils::SteadyClock::time_point> res;
  utils::TimeCost time_cost{__FUNCTION__, false};
  rp(i, cnt) {
    res = utils::NO_INLINE_FUNC(
        [&] { return async_time.clock().steady_time_point(); });
  }
  const auto dur = time_cost.Duration();
  double total_cnt = cnt;
  double sec = dur.count() / 1e9;
  time_cost.Show();
  FMSGLN("avg: {}, qps: {:.3f}", dur / total_cnt, total_cnt / sec);
}

static void bench_stl_time(const size_t cnt) {
  std::atomic<utils::SteadyClock::time_point> res;
  utils::TimeCost time_cost{__FUNCTION__, false};
  rp(i, cnt) {
    res = utils::NO_INLINE_FUNC([&] { return utils::SteadyClock::now(); });
  }
  const auto dur = time_cost.Duration();
  double total_cnt = cnt;
  double sec = dur.count() / 1e9;
  time_cost.Show();
  FMSGLN("avg: {}, qps: {:.3f}", dur / cnt, total_cnt / sec);
}

template <typename Timer>
void bench_timer_task(Timer &timer, const size_t cnt, const size_t parallel) {
  std::atomic_uint64_t res = cnt * parallel;
  utils::AtomicNotifier notifer;
  utils::TimeCost time_cost{__FUNCTION__, false};
  std::list<std::thread> threads;
  utils::Waiter waiter;
  const auto delay = utils::Milliseconds{100};
  rp(c, parallel) {
    threads.emplace_back([&] {
      waiter.Wait();
      rp(i, cnt) {
        timer.Schedule(
            [&](bool) {
              if ((--res) == 0)
                notifer.Wake();
            },
            delay);
      }
    });
  }
  {
    waiter.WakeAll();
    time_cost.Reset();
    notifer.BlockedWait();
  }
  auto dur = time_cost.Duration();
  dur -= delay;
  RUNTIME_ASSERT_EQ(res, 0);
  double total_cnt = cnt * parallel;
  double sec = dur.count() / 1e9;
  time_cost.Show();
  FMSGLN("avg: {}, qps: {:.3f}", dur / total_cnt, total_cnt / sec);
  for (auto &&t : threads)
    t.join();
}

static void bench_timer(int argc, char **argv) {
  const size_t cnt = 1e8;
  const size_t timer_cnt = 4e6;
  const size_t timer_parallel = 6;

  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";
  auto &&fn_bench_timer_task = [] {
    LOG_INFO("using heap timer");
    utils::Timer timer(true, timer_parallel);
    bench_timer_task(timer, timer_cnt, timer_parallel);
  };
  auto &&fn_bench_timer_wheel_task = [] {
    LOG_INFO("using wheel timer");
    utils::chrono::AsyncClock async_time(utils::Milliseconds{1}, true);
    utils::WheelTimerRunner timer(async_time, timer_parallel);
    bench_timer_task(timer, timer_cnt, timer_parallel);
  };
  auto &&fn_bench_timer_async = [] { bench_async_time(cnt); };
  auto &&fn_bench_timer_stl = [] { bench_stl_time(cnt); };

#define M(name) fn_bench_timer_##name();
  if (mode_str == "ALL") {
    M(async)
    M(stl)
    M(task)
    M(wheel_task)
  } else if (mode_str == "ASYNC") {
    M(async)
  } else if (mode_str == "STL") {
    M(stl)
  } else if (mode_str == "TASK") {
    M(task)
  } else if (mode_str == "WHEEL") {
    M(wheel_task)
  }
#undef M
}

} // namespace bench

FUNC_FACTORY_REGISTER("TIMER", bench::bench_timer)
