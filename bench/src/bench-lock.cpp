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

  const auto label =
      fmt::format(std::locale("en_US.UTF-8"), "{}-{:L}-{}", name, n, parallel);
  utils::Waiter waiter;
  utils::Waiter waiter2;
  std::atomic_uint64_t r{parallel};
  std::list<std::thread> threads;
  rp(i, parallel) {
    threads.emplace_back([&] {
      waiter.Wait();
      rp(j, n) {
        lock.RunWithLock([&] { inc(x); });
      }
      if (0 == (--r)) {
        waiter2.WakeOne();
      }
    });
  }
  {
    utils::TimeCost time_cost(label, false);
    waiter.WakeAll();
    waiter2.Wait();
    auto dur = time_cost.Duration();
    time_cost.Show();
    bench::ShowDurAvgAndOps(dur, n);
  }
  for (auto &&t : threads)
    t.join();

  if constexpr (std::is_same_v<TestLockNone, LockType>) {
    RUNTIME_ASSERT_NE(n * threads.size(), x);
  } else {
    RUNTIME_ASSERT_EQ(n * threads.size(), x);
  }
}

struct TestMutexLock : utils::MutexLockWrap {
  template <typename F> inline auto RunWithLock(F &&f) const {
    return RunWithMutexLock(std::forward<F>(f));
  }
};

static void bench_lock(int argc, char **argv) {

  const size_t n = 1e8;
  const size_t parallel = 4;

  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";

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
