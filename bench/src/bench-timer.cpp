#include <bench/bench.h>
#include <utils/timer.hpp>

namespace {

static void bench_async_clock(const size_t cnt) {
  auto &async_time = utils::chrono::AsyncClock::GlobalInstance();
  std::atomic<utils::SteadyClock::time_point> res;
  utils::TimeCost time_cost{__FUNCTION__, false};
  rp(i, cnt) {
    res = utils::NO_INLINE_FUNC(
        [&] { return async_time.clock().steady_time_point(); });
  }
  const auto dur = time_cost.Duration();
  time_cost.Show();
  bench::ShowDurAvgAndOps(dur, cnt);
}

static void bench_stl_clock(const size_t cnt) {
  std::atomic<utils::SteadyClock::time_point> res;
  utils::TimeCost time_cost{__FUNCTION__, false};
  rp(i, cnt) {
    res = utils::NO_INLINE_FUNC([&] { return utils::SteadyClock::now(); });
  }
  const auto dur = time_cost.Duration();
  time_cost.Show();
  bench::ShowDurAvgAndOps(dur, cnt);
}

template <typename Timer>
void bench_timer_task(Timer &timer, const size_t cnt, const size_t parallel,
                      std::string_view _label) {
  auto &&label = fmt::format("{}({})", __FUNCTION__, _label);
  LOG_INFO("running {}", label);
  std::atomic_uint64_t res = cnt * parallel;
  utils::AtomicNotifier notifer;
  utils::TimeCost time_cost{label, false};
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
  RUNTIME_ASSERT_EQ(res.load(std::memory_order_seq_cst), 0);
  time_cost.Show();
  bench::ShowDurAvgAndOps(dur, cnt);
  for (auto &&t : threads)
    t.join();
}

static void bench_timer(int argc, char **argv) {
  const size_t cnt = 1e8;
  const size_t timer_cnt = 4e6;
  const size_t timer_parallel = 6;

  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";
  auto &&fn_bench_timer_task = [] {
    const auto &&label = std::string_view{"heap-timer"};
    utils::Timer timer(true, timer_parallel);
    bench_timer_task(timer, timer_cnt, timer_parallel, label);
  };
  auto &&fn_bench_timer_wheel_task = [] {
    const auto &&label = std::string_view{"wheel-timer"};
    utils::chrono::AsyncClock async_time(utils::Milliseconds{1}, true);
    utils::WheelTimerRunner timer(async_time, timer_parallel);
    bench_timer_task(timer, timer_cnt, timer_parallel, label);
  };
  auto &&fn_bench_timer_async = [] {
    LOG_INFO("using async clock");
    bench_async_clock(cnt);
  };
  auto &&fn_bench_timer_stl = [] {
    LOG_INFO("using stl clock");
    bench_stl_clock(cnt);
  };
  auto &&fn_bench_timer_yield = [] {
    constexpr size_t n = 1e6;
    LOG_INFO("running yield");
    auto start = std::chrono::high_resolution_clock::now();
    {
      rp(i, n) { std::this_thread::yield(); }
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    bench::ShowDurAvgAndOps(elapsed, n);
  };
  auto &&fn_bench_timer_notifer = [] {
    constexpr size_t n = 1e7;
    LOG_INFO("running notifer");
    std::atomic_flag f;
    auto start = std::chrono::high_resolution_clock::now();
    {
      rp(i, n) utils::NO_INLINE_FUNC([&] { f.notify_one(); });
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    bench::ShowDurAvgAndOps(elapsed, n);
  };

#define M(name) fn_bench_timer_##name();
  bench::FuncMap data = {
      {"ALL",
       [&] {
         M(async)
         M(stl)
         M(task)
         M(wheel_task)
         M(yield)
         M(notifer)
       }},
      {"ASYNC", [&] { M(async) }},
      {"STL", [&] { M(stl) }},
      {"TASK", [&] { M(task) }},
      {"WHEEL", [&] { M(wheel_task) }},
      {"YIELD", [&] { M(yield) }},
      {"NOTIFER", [&] { M(notifer) }},
  };
#undef M

  bench::ExecFuncMap(data, mode_str);
}

} // namespace

FUNC_FACTORY_REGISTER("TIMER", bench_timer)
