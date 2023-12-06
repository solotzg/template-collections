#include <bench/bench.h>

namespace bench {

NO_INLINE static void inc(uint64_t &x) { x += 1; }

struct TestLockNone {
  template <typename F> inline auto RunWithLock(F &&f) const { return f(); }
};

template <typename LockType>
static void bench_lock_impl(const size_t n, const size_t parallel,
                            std::string_view name) {
  LockType lock{};
  uint64_t x{};

  const auto label = fmt::format("{} concurrency={}", name, parallel);

  utils::Waiter ready_flag, waiter, finish;
  std::atomic_uint64_t tcnt = parallel;
  std::atomic_uint64_t tcnt_ready = parallel;
  std::list<std::thread> threads;
  SCOPE_EXIT({
    for (auto &&t : threads) {
      t.join();
    }
  });
  rp(i, parallel) {
    threads.emplace_back([&] {
      {
        if (--tcnt_ready == 0) {
          ready_flag.WakeOne();
        }
        waiter.Wait();
      }
      rp(j, n) {
        lock.RunWithLock([&] { inc(x); });
      }
      if (--tcnt == 0) {
        finish.WakeOne();
      }
    });
  }
  const auto expect_n = n * threads.size();
  {
    utils::TimeCost time_cost(label, false);
    {
      ready_flag.Wait();
      waiter.WakeAll();
      finish.Wait();
    }
    auto dur = time_cost.Duration();
    time_cost.Show();
    bench::ShowDurAvgAndOps(dur, expect_n);
  }

  if constexpr (std::is_same_v<TestLockNone, LockType>) {
    if (parallel != 1)
      RUNTIME_ASSERT_NE(expect_n, x);
    else
      RUNTIME_ASSERT_EQ(expect_n, x);
  } else {
    RUNTIME_ASSERT_EQ(expect_n, x);
  }
}

struct TestMutexLock : utils::MutexLockWrap {
  template <typename F> inline auto RunWithLock(F &&f) const {
    return RunWithMutexLock(std::forward<F>(f));
  }
};

static void bench_lock(int argc, char **argv) {

  const size_t n = 1e8;
  size_t parallel = 4;

  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";
  if (argc > 1) {
    parallel = std::stoi(argv[1]);
  }

  auto &&fn_bench_mutex = [&] {
    bench_lock_impl<TestMutexLock>(n, parallel, "MUTEX");
  };
  auto &&fn_bench_spin = [&] {
    bench_lock_impl<utils::SpinLockWrap>(n, parallel, "SPIN");
  };
  auto &&fn_bench_none = [&] {
    bench_lock_impl<TestLockNone>(n, parallel, "NONE");
  };

#define M(name) fn_bench_##name();
  if (mode_str == "ALL") {
    M(mutex)
    M(spin)
    M(none)
  } else if (mode_str == "MUTEX") {
    M(mutex)
  } else if (mode_str == "SPIN") {
    M(spin)
  } else if (mode_str == "NONE") {
    M(none)
  }
#undef M
}

} // namespace bench

FUNC_FACTORY_REGISTER("LOCK", bench::bench_lock)
