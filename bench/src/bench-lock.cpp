#include <bench/bench.h>

namespace {

void inc(uint64_t &x) { x += 1; }
void inc(std::atomic<uint64_t> &x) {
  x.fetch_add(1, std::memory_order_acq_rel);
}
void exchg(utils::AtomicNotifier &notifer) {
  notifer.WaitAndExchangeAwake(true);
}

struct TestLockNone {
  template <typename F> inline auto RunWithLock(F &&f) const { return f(); }
};

template <typename LockType>
static void bench_lock_impl(const size_t n, const size_t parallel,
                            std::string_view name) {
  LockType lock{};
  utils::CPUCacheLineAligned<uint64_t> x;

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
      rp(j, n)
          utils::NO_INLINE_FUNC([&] { lock.RunWithLock([&] { inc(*x); }); });
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
    if (parallel == 1)
      RUNTIME_ASSERT_EQ(expect_n, *x);
  } else {
    RUNTIME_ASSERT_EQ(expect_n, *x);
  }
}

static void bench_exchange(const size_t n, const size_t parallel,
                           std::string_view name) {
  const auto label = fmt::format("{} concurrency={}", name, parallel);

  utils::Waiter ready_flag, waiter, finish;
  std::atomic_uint64_t tcnt = parallel;
  std::atomic_uint64_t tcnt_ready = parallel;
  utils::AtomicNotifier notifer;
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
      rp(j, n) utils::NO_INLINE_FUNC([&] { exchg(notifer); });
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
}

static void bench_fetch_add(const size_t n, const size_t parallel,
                            std::string_view name) {
  utils::CPUCacheLineAligned<std::atomic<uint64_t>> x{};

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
      rp(j, n) utils::NO_INLINE_FUNC([&] { inc(*x); });
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

  RUNTIME_ASSERT_EQ(n * threads.size(), x->load());
}

struct TestMutexLock : utils::MutexLockWrap {
  template <typename F> inline auto RunWithLock(F &&f) const {
    return RunWithMutexLock(std::forward<F>(f));
  }
};

static void bench_lock(int argc, char **argv) {

  const size_t n = 1e8;
  size_t parallel = 2;

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
  auto &&fn_bench_exchg = [&] { bench_exchange(n, parallel, "EXCHG"); };
  auto &&fn_bench_add = [&] { bench_fetch_add(n, parallel, "ADD"); };

#define M(name) fn_bench_##name();
  bench::FuncMap data = {
      {"ALL",
       [&] {
         M(mutex)
         M(spin)
         M(none)
         M(exchg)
         M(add)
       }},
      {"MUTEX", [&] { M(mutex) }},
      {"SPIN", [&] { M(spin) }},
      {"NONE", [&] { M(none) }},
      {"EXCHG", [&] { M(exchg) }},
      {"ADD", [&] { M(add) }},
  };
#undef M

  bench::ExecFuncMap(data, mode_str);
}

} // namespace

FUNC_FACTORY_REGISTER("LOCK", bench_lock)
