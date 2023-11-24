#include "bench/bench.h"
#include "utils/mpsc.hpp"

namespace bench {

using BenchElementType = int;
using BenchNode =
    utils::AlignedStruct<BenchElementType, alignof(BenchElementType)>;
// using BenchNode =
//     AlignedStruct<BenchElementType, BasicConfig::CPU_CACHE_LINE_SIZE>;

template <typename T> struct MPMCNormal {
  MPMCNormal(size_t producer_size, size_t producer_cap)
      : producer_size_(producer_size) {
    queues_.Allocate(producer_size_);
    rp(i, producer_size_) queues_.New(i, producer_cap);
  }
  ~MPMCNormal() {
    rp(i, producer_size_) queues_.Del(i);
    queues_.Deallocate(producer_size_);
  }
  bool TryPut(T &&v, size_t index) {
    return queues_[index].emplace(std::move(v));
  }
  template <typename CB> bool TryGet(CB &&cb) {
    const auto ori_index = round_robin_index();
    auto &&func = [&](size_t end) -> std::optional<T> {
      std::optional<T> res;
      for (; round_robin_index() < end; ++round_robin_index()) {
        auto &queue = queues_[round_robin_index()];
        auto res = queue.pop();
        if (res)
          return res;
      }
      return std::nullopt;
    };
    auto res = func(producer_size_);
    if (!res) {
      round_robin_index() = 0;
      res = func(ori_index);
    }
    if (res) {
      cb(std::move(*res));
      return true;
    }
    return false;
  }

private:
  size_t &round_robin_index() { return *round_robin_index_; }
  const size_t &round_robin_index() const { return *round_robin_index_; }

  struct Data : utils::MutexLockWrap {
    Data(size_t cap) : cap_(cap), cap_mask_(cap_ - 1) {
      RUNTIME_ASSERT_EQ(cap, utils::NextPow2(cap));
      inner_.Allocate(cap);
    }
    ~Data() {
      rp(i, size_) inner_.Del((i + begin_) & cap_mask_);
      inner_.Deallocate(cap_);
    }
    T &operator[](size_t index) { return inner_[index]; }
    const T &operator[](size_t index) const { return inner_[index]; }

    size_t capacity() const { return cap_; }

    bool emplace(T &&val) {
      return RunWithMutexLock([&] {
        if (size_ >= cap_)
          return false;
        inner_.New(end_, std::move(val));
        end_ = (end_ + 1) & cap_mask_;
        ++size_;
        return true;
      });
    }

    std::optional<T> pop() {
      return RunWithMutexLock([&]() -> std::optional<T> {
        if (0 == size_)
          return std::nullopt;
        auto index = begin_;
        begin_ = (begin_ + 1) & cap_mask_;
        --size_;
        SCOPE_EXIT({ inner_.Del(index); });
        return std::move(inner_[index]);
      });
    }

  private:
    const size_t cap_{};
    const size_t cap_mask_{};
    size_t begin_{};
    size_t end_{};
    size_t size_{};
    utils::ConstSizeArray<T> inner_;
  };

  const size_t producer_size_;
  utils::ConstSizeArray<Data> queues_;
  utils::AlignedStruct<size_t, utils::BasicConfig::CPU_CACHE_LINE_SIZE>
      round_robin_index_{};
};

NO_INLINE
static void bench_mpsc_normal_stl(size_t producer_size, size_t test_loop,
                                  size_t producer_cap) {
  MPMCNormal<BenchNode> mpmc_worker(producer_size, producer_cap);
  std::list<std::thread> producer_runners;
  utils::Waiter waiter;
  utils::AlignedStruct<std::atomic_uint64_t,
                       utils::BasicConfig::CPU_CACHE_LINE_SIZE>
      tol_full_cnt{};

  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      auto full_cnt = 0;
      waiter.Wait();
      for (int i = 0; i < test_loop; ++i) {
        while (!mpmc_worker.TryPut(int(id), id)) {
          ++full_cnt;
          std::this_thread::yield();
        }
      }
      *tol_full_cnt += full_cnt;
    });
  }
  waiter.WakeAll();

  size_t empty_cnt = 0;
  SCOPE_EXIT(
      { FMSGLN("    [full_cnt={}][empty_cnt={}]", *tol_full_cnt, empty_cnt); });

  utils::TimeCost time_cost{__FUNCTION__};

  std::atomic_int64_t res = 0;
  std::atomic_int64_t cnt = 0;
  const int64_t expect_res =
      producer_size * (producer_size - 1) / 2 * test_loop;
  for (;;) {
    auto x = mpmc_worker.TryGet([&](BenchNode &&v) { res += *v; });
    if (!x) {
      ++empty_cnt;
      std::this_thread::yield();
      continue;
    }
    cnt += x;
    if (cnt >= producer_size * test_loop) {
      break;
    }
  }

  for (auto &&t : producer_runners)
    t.join();
  RUNTIME_ASSERT(res == expect_res);
  RUNTIME_ASSERT(cnt == producer_size * test_loop);
}

NO_INLINE
static void bench_mpsc_spin_loop(size_t producer_size, size_t test_loop,
                                 size_t producer_cap) {
  utils::MPSCQueueWithNotifer<utils::MPSCWorker<BenchNode>> mpsc_worker(
      producer_size, producer_cap);
  std::list<std::thread> producer_runners;
  utils::Waiter waiter;
  utils::AlignedStruct<std::atomic_uint64_t,
                       utils::BasicConfig::CPU_CACHE_LINE_SIZE>
      tol_full_cnt{};
  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      auto full_cnt = 0;
      waiter.Wait();
      for (int i = 0; i < test_loop; ++i) {
        while (!mpsc_worker.TryPut(int(id), id)) {
          ++full_cnt;
          std::this_thread::yield();
        }
      }
      *tol_full_cnt += full_cnt;
    });
  }
  waiter.WakeAll();

  size_t empty_cnt = 0;
  SCOPE_EXIT(
      { FMSGLN("    [full_cnt={}][empty_cnt={}]", *tol_full_cnt, empty_cnt); });

  utils::TimeCost time_cost{__FUNCTION__};

  std::atomic_int64_t res = 0;
  std::atomic_int64_t cnt = 0;
  const int64_t expect_res =
      producer_size * (producer_size - 1) / 2 * test_loop;
  for (;;) {
    auto x = mpsc_worker.TryGet(
        [&](BenchNode &&v, size_t) {
          res += *v;
          // MSGLN(v);
        },
        [](size_t) {}, 1024 * 4);
    if (!x) {
      ++empty_cnt;
      std::this_thread::yield();
      continue;
    }
    cnt += x;
    if (cnt >= producer_size * test_loop) {
      break;
    }
  }

  for (auto &&t : producer_runners)
    t.join();
  RUNTIME_ASSERT(res == expect_res);
  RUNTIME_ASSERT(cnt == producer_size * test_loop);
}

NO_INLINE
static void bench_mpsc_awake(size_t producer_size, size_t test_loop,
                             size_t producer_cap) {
  utils::Notifier mpsc_notifier;
  utils::MPSCQueueWithNotifer<utils::MPSCWorker<BenchNode>> mpsc_worker(
      producer_size, producer_cap);
  std::list<std::thread> producer_runners;
  utils::Waiter waiter;
  utils::AlignedStruct<std::atomic_uint64_t,
                       utils::BasicConfig::CPU_CACHE_LINE_SIZE>
      tol_full_cnt{};
  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      waiter.Wait();
      size_t full_cnt = 0;
      for (int i = 0; i < test_loop; ++i) {
        auto res = mpsc_worker.Put(int(id), id, [&]() {
          mpsc_notifier.Wake();
          ++full_cnt;
        });
        RUNTIME_ASSERT(res);
        mpsc_notifier.Wake();
      }
      *tol_full_cnt += full_cnt;
    });
  }
  waiter.WakeAll();

  size_t empty_cnt = 0;
  SCOPE_EXIT(
      { FMSGLN("    [full_cnt={}][empty_cnt={}]", *tol_full_cnt, empty_cnt); });
  utils::TimeCost time_cost{__FUNCTION__};

  std::atomic_int64_t res = 0;
  std::atomic_int64_t cnt = 0;
  const int64_t expect_res =
      producer_size * (producer_size - 1) / 2 * test_loop;
  for (;;) {
    auto x = mpsc_worker.Get(
        [&](BenchNode &&v, [[maybe_unused]] size_t producer_index) {
          res += *v;
          // It may affect performance if producer is much faster than customer.
          // mpsc_worker.WakeProducer(producer_index);
        },
        [&](size_t empty_producer_index) {
          mpsc_worker.WakeProducer(empty_producer_index);
          ++empty_cnt;
          //  MSGLN("wake producer", empty_producer_index);
        },
        4 * 1024, mpsc_notifier);
    cnt += x;
    if (cnt >= producer_size * test_loop) {
      break;
    }
    mpsc_worker.WakeAllProducers();
  }

  for (auto &&t : producer_runners)
    t.join();
  RUNTIME_ASSERT(res == expect_res);
  RUNTIME_ASSERT(cnt == producer_size * test_loop);
}

NO_INLINE
static void bench_mpsc_fastbin_alloc(size_t producer_size, size_t test_loop,
                                     size_t producer_cap) {
  utils::Notifier mpsc_notifier;
  utils::MPSCQueueFastBinAlloc<BenchNode> mpsc_worker(
      producer_size, producer_cap * producer_size);
  std::list<std::thread> producer_runners;
  utils::Waiter waiter;
  utils::AlignedStruct<std::atomic_uint64_t,
                       utils::BasicConfig::CPU_CACHE_LINE_SIZE>
      tol_full_cnt{};
  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      waiter.Wait();
      size_t full_cnt = 0;
      for (int i = 0; i < test_loop; ++i) {
        while (!mpsc_worker.TryPush(id, BenchElementType(id))) {
          ++full_cnt;
          std::this_thread::yield();
        }
      }
      *tol_full_cnt += full_cnt;
    });
  }
  waiter.WakeAll();

  size_t empty_cnt = 0;
  SCOPE_EXIT(
      { FMSGLN("    [full_cnt={}][empty_cnt={}]", *tol_full_cnt, empty_cnt); });
  utils::TimeCost time_cost{__FUNCTION__};

  std::atomic_int64_t res = 0;
  std::atomic_int64_t cnt = 0;
  const int64_t expect_res =
      producer_size * (producer_size - 1) / 2 * test_loop;
  for (;;) {
    auto x = mpsc_worker.Consume([&](auto &&v) { res += *v; }, 1024 * 4);
    if (!x) {
      ++empty_cnt;
      std::this_thread::yield();
      continue;
    }
    cnt += x;
    if (cnt >= producer_size * test_loop) {
      break;
    }
  }

  for (auto &&t : producer_runners)
    t.join();
  RUNTIME_ASSERT(res == expect_res);
  RUNTIME_ASSERT(cnt == producer_size * test_loop);
}

static void bench_mpsc(int argc, char **argv) {
#define M(fn_name) fn_name(7, 1024 * 1024 * 4, 1024)
  std::string mode_name = "";
  std::string mode_str = "";
  if (argc > 0) {
    mode_name = argv[0];
    mode_str = utils::ToUpper(std::string_view{mode_name});
  }
  std::unordered_map<std::string, std::function<void()>> data = {
      {"ALL",
       [] {
         M(bench_mpsc_normal_stl);
         M(bench_mpsc_awake);
         M(bench_mpsc_spin_loop);
         M(bench_mpsc_fastbin_alloc);
       }},
      {"STL", [] { M(bench_mpsc_normal_stl); }},
      {"AWAKE", [] { M(bench_mpsc_awake); }},
      {"SPIN", [] { M(bench_mpsc_spin_loop); }},
      {"FASTBIN", [] { M(bench_mpsc_fastbin_alloc); }},
  };
  if (auto it = data.find(mode_str); it != data.end()) {
    it->second();
  } else {
    FMSGLN("Unknown mode `{}`, option:", mode_name);
    for (auto &&e : data) {
      FMSGLN("  {}", e.first);
    }
    return;
  }

#undef M
}

} // namespace bench

FUNC_FACTORY_REGISTER("MPSC", bench::bench_mpsc)
