#include "utils/mpsc_base.hpp"

namespace bench {
template <typename T> struct TestNode {
  TestNode(const TestNode &) = delete;
  TestNode(T &&val) : val_(std::move(val)) { inc_cnt(); }
  ~TestNode() { dec_cnt(); }
  TestNode(TestNode &&src) {
    inc_cnt();
    val_ = std::move(src.val_);
  }
  TestNode &operator=(TestNode &&src) {
    inc_cnt();
    val_ = std::move(src.val_);
    return *this;
  }
  static std::atomic_uint64_t &test_node_cnt() {
    static std::atomic_uint64_t data{};
    return data;
  }
  static void inc_cnt() { test_node_cnt()++; }
  static void dec_cnt() { test_node_cnt()--; }
  T val_{};
};

using BenchElementType = int;
using BenchNode = AlignedStruct<BenchElementType, alignof(BenchElementType)>;
// using BenchNode =
//     AlignedStruct<BenchElementType, BasicConfig::CPU_CACHE_LINE_SIZE>;

template <typename T> struct MPMCNormal : MutexLockWrap {
  MPMCNormal(size_t producer_size, size_t producer_cap)
      : producer_size_(producer_size), producer_cap_(producer_cap) {
    queues_ = std::allocator<Data>{}.allocate(producer_size_);
    rp(i, producer_size_) new (queues_ + i) Data(producer_cap);
  }
  ~MPMCNormal() {
    rp(i, producer_size_) queues_[i].~Data();
    std::allocator<Data>{}.deallocate(queues_, producer_size_);
  }
  bool TryPut(T &&v, size_t index) {
    return queues_[index].emplace(std::move(v));
  }
  template <typename CB> bool TryGet(CB &&cb) {
    const auto ori_index = round_robin_index_;
    auto &&func = [&](size_t end) -> std::optional<T> {
      std::optional<T> res;
      for (; round_robin_index_ < end; ++round_robin_index_) {
        auto &queue = queues_[round_robin_index_];
        auto res = queue.pop();
        if (res)
          return res;
      }
      return std::nullopt;
    };
    auto res = func(producer_size_);
    if (!res) {
      round_robin_index_ = 0;
      res = func(ori_index);
    }
    if (res) {
      cb(std::move(*res));
      return true;
    }
    return false;
  }

private:
  const size_t producer_cap_;
  struct Data : MutexLockWrap {
    Data(size_t cap) : cap_(cap), cap_mask_(cap_ - 1) {
      RUNTIME_ASSERT_EQ(cap, NextPow2(cap));
      inner_ = std::allocator<T>{}.allocate(cap);
    }
    ~Data() {
      rp(i, size_) inner_[(i + begin_) & cap_mask_].~T();
      std::allocator<T>{}.deallocate(inner_, cap_);
    }
    T &operator[](size_t index) { return inner_[index]; }
    const T &operator[](size_t index) const { return inner_[index]; }

    size_t capacity() const { return cap_; }

    bool emplace(T &&val) {
      auto lock = GenLockGuard();
      if (size_ >= cap_)
        return false;
      new (inner_ + end_) T(std::move(val));
      end_ = (end_ + 1) & cap_mask_;
      ++size_;
      return true;
    }

    std::optional<T> pop() {
      auto lock = GenLockGuard();
      if (0 == size_)
        return std::nullopt;
      auto index = begin_;
      begin_ = (begin_ + 1) & cap_mask_;
      --size_;
      return std::move(inner_[index]);
    }

  private:
    const size_t cap_{};
    const size_t cap_mask_{};
    size_t begin_{};
    size_t end_{};
    size_t size_{};
    T *inner_{};
  };

  const size_t producer_size_;
  Data *queues_;
  alignas(BasicConfig::CPU_CACHE_LINE_SIZE) size_t round_robin_index_{};
};

void bench_mpsc_normal_stl(size_t producer_size, size_t test_loop,
                           size_t producer_cap) {
  MPMCNormal<BenchNode> mpmc_worker(producer_size, producer_cap);
  std::list<std::thread> producer_runners;
  Waiter waiter;
  AlignedStruct<std::atomic_uint64_t, BasicConfig::CPU_CACHE_LINE_SIZE>
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

  TimeCost time_cost{__FUNCTION__};

  volatile int64_t res = 0;
  volatile int64_t cnt = 0;
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

void bench_mpsc_spin_loop(size_t producer_size, size_t test_loop,
                          size_t producer_cap) {
  MPSCQueueWithNotifer<MPSCWorker<BenchNode>> mpsc_worker(producer_size,
                                                          producer_cap);
  std::list<std::thread> producer_runners;
  Waiter waiter;
  AlignedStruct<std::atomic_uint64_t, BasicConfig::CPU_CACHE_LINE_SIZE>
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

  TimeCost time_cost{__FUNCTION__};

  volatile int64_t res = 0;
  volatile int64_t cnt = 0;
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

void bench_mpsc_awake(size_t producer_size, size_t test_loop,
                      size_t producer_cap) {
  MPSCQueueWithNotifer<MPSCWorker<BenchNode>> mpsc_worker(producer_size,
                                                          producer_cap);
  std::list<std::thread> producer_runners;
  Waiter waiter;
  AlignedStruct<std::atomic_uint64_t, BasicConfig::CPU_CACHE_LINE_SIZE>
      tol_full_cnt{};
  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      waiter.Wait();
      size_t full_cnt = 0;
      for (int i = 0; i < test_loop; ++i) {
        auto res =
            mpsc_worker.Put(int(id), id, std::chrono::seconds{8192}, [&]() {
              mpsc_worker.WakeCustomer();
              ++full_cnt;
            });
        RUNTIME_ASSERT(res);
        mpsc_worker.WakeCustomer();
      }
      *tol_full_cnt += full_cnt;
    });
  }
  waiter.WakeAll();

  size_t empty_cnt = 0;
  SCOPE_EXIT(
      { FMSGLN("    [full_cnt={}][empty_cnt={}]", *tol_full_cnt, empty_cnt); });
  TimeCost time_cost{__FUNCTION__};

  volatile int64_t res = 0;
  volatile int64_t cnt = 0;
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
        std::chrono::seconds{8192}, 4 * 1024);
    cnt += x;
    if (cnt >= producer_size * test_loop) {
      break;
    }
    rp(producer_index, producer_size) mpsc_worker.WakeProducer(producer_index);
  }

  for (auto &&t : producer_runners)
    t.join();
  RUNTIME_ASSERT(res == expect_res);
  RUNTIME_ASSERT(cnt == producer_size * test_loop);
}

enum class Mode {
  ALL,
  STL,
  AWAKE,
  SPIN,
};

void bench_mpsc(size_t producer_size, size_t test_loop, size_t producer_cap,
                Mode mode) {
  RUNTIME_ASSERT(producer_size + 1 <= std::thread::hardware_concurrency());
#define M(fn_name) fn_name(producer_size, test_loop, producer_cap)
  switch (mode) {
  case Mode::ALL: {
    M(bench_mpsc_normal_stl);
    M(bench_mpsc_awake);
    M(bench_mpsc_spin_loop);
    break;
  }
  case Mode::STL: {
    M(bench_mpsc_normal_stl);
    break;
  }
  case Mode::AWAKE: {
    M(bench_mpsc_awake);
    break;
  }
  case Mode::SPIN: {
    M(bench_mpsc_spin_loop);
    break;
  }
  }
#undef M
}

} // namespace bench

int main(int argc, char **argv) {
  ShowBuildInfo(std::cout);
  MSGLN("------");
  bench::Mode mode{};
  if (argc > 1) {
    if (std::string_view sv = argv[1]; sv == "ALL") {
      mode = bench::Mode::ALL;
    } else if (sv == "STL") {
      mode = bench::Mode::STL;
    } else if (sv == "AWAKE") {
      mode = bench::Mode::AWAKE;
    } else if (sv == "SPIN") {
      mode = bench::Mode::SPIN;
    } else {
      return 0;
    }
  }
  bench::bench_mpsc(7, 1024 * 1024 * 4, 1024, mode);
}