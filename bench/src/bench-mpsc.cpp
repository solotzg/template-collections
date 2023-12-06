#include <bench/bench.h>
#include <utils/mpsc.hpp>

#define DEF_THREAD_YIELD_N 10

namespace {

#define GEN_TIME_COST(name, ...) utils::TimeCost name(fmt::format(__VA_ARGS__));

using BenchElementType = int;
using BenchNode =
    utils::AlignedStruct<BenchElementType, alignof(BenchElementType)>;
// using BenchNode =
//     AlignedStruct<BenchElementType, BasicConfig::CPU_CACHE_LINE_SIZE>;

template <typename T> struct NormalSPSCQueue {
  using Element = T;

  explicit NormalSPSCQueue(uint32_t cap)
      : max_num_(utils::NextPow2(cap)), buffer_mask_(max_num_ - 1) {
    buff_.Allocate(max_num_);
  }

  NormalSPSCQueue(const NormalSPSCQueue &) = delete;

  inline size_t next_pos(size_t pos) const { return (pos + 1) & buffer_mask_; }

  ~NormalSPSCQueue() {
    for (size_t cur = head(); cur != tail(); cur = next_pos(cur)) {
      GetAtImpl(cur, [&](T &) constexpr {});
    }
    head() = tail() = 0;
    buff_.Deallocate(max_num_);
  }

  static constexpr size_t npos = -1;

  // for consumer
  size_t ConsumerCheckEmpty() {
    const auto _head = head().load(std::memory_order_relaxed);
    if (_head == latest_tail()) {
      return npos;
    }
    return _head;
  }

  // for producer
  size_t ProducerCheckFull() {
    const auto _tail = tail().load(std::memory_order_relaxed);
    const auto next = next_pos(_tail);
    if (next == latest_head()) {
      return npos;
    }
    return _tail;
  }

  size_t latest_head() const { return head().load(std::memory_order_acquire); }
  size_t latest_tail() const { return tail().load(std::memory_order_acquire); }

  // consumer gets from head
  std::optional<T> Get() {
    const size_t pos = ConsumerCheckEmpty();
    if (pos == npos) {
      return std::nullopt;
    }
    std::optional<T> res{};
    GetAtImpl(pos, [&](T &v) { res.emplace(std::move(v)); });
    PublishHead(next_pos(pos));
    return res;
  }

  // producer puts to tail
  bool Put(T &&data) {
    const size_t pos = ProducerCheckFull();
    if (pos == npos) {
      return false;
    }
    buff_.New(pos, std::move(data));
    PublishTail(next_pos(pos));
    return true;
  }

private:
  template <typename F> void GetAtImpl(size_t index, F &&f) {
    static_assert(std::is_destructible_v<T>);
    f(buff_[index]);
    buff_.Del(index);
  }

  std::atomic_size_t &head() { return *head_; }
  const std::atomic_size_t &head() const { return *head_; }
  std::atomic_size_t &tail() { return *tail_; }
  const std::atomic_size_t &tail() const { return *tail_; }
  void PublishTail(size_t pos) { tail().store(pos, std::memory_order_release); }
  void PublishHead(size_t pos) { head().store(pos, std::memory_order_release); }

private:
  const size_t max_num_;
  const size_t buffer_mask_;
  utils::ConstSizeArray<T, std::allocator<T>> buff_;
  // Aligned to CPU cache line size
  // consumer get
  utils::CPUCacheLineAligned<std::atomic_size_t> head_;
  // producer put
  utils::CPUCacheLineAligned<std::atomic_size_t> tail_;
};

template <size_t BatchSize = 512> struct BatchCounter {
  template <typename F> void run(F &&f) {
    if (++batch_ == BatchSize) {
      f();
      batch_ = 0;
    }
  }
  size_t batch_ = 0;
};

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
  utils::CPUCacheLineAligned<size_t> round_robin_index_{};
};

static void bench_mpsc_normal_stl(std::string_view label, size_t producer_size,
                                  size_t test_loop, size_t producer_cap) {
  MPMCNormal<BenchNode> mpmc_worker(producer_size, producer_cap);
  std::list<std::thread> producer_runners;
  utils::Waiter waiter;
  utils::CPUCacheLineAligned<std::atomic_uint64_t> tol_full_cnt{};
  std::vector<utils::AtomicNotifier> producer_notifiers(producer_size);
  utils::AtomicNotifier consumer_notifier;
  using BC = BatchCounter<128>;

  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      auto full_cnt = 0;
      BC bc;

      waiter.Wait();
      for (int i = 0; i < test_loop; ++i) {
        for (;;) {
          if (!mpmc_worker.TryPut(int(id), id)) {
            ++full_cnt;
            consumer_notifier.Wake();
            producer_notifiers[id].BlockedWait();
          } else {
            break;
          }
        }
        bc.run([&] { consumer_notifier.Wake(); });
      }
      *tol_full_cnt += full_cnt;
    });
  }
  SCOPE_EXIT({
    for (auto &&t : producer_runners)
      t.join();
  });

  size_t empty_cnt = 0;
  utils::TimeCost::Clock::duration dur;
  SCOPE_EXIT({
    FMSGLN("    [full_cnt={}][empty_cnt={}]", tol_full_cnt->load(), empty_cnt);
    bench::ShowDurAvgAndOps(dur, producer_size * test_loop);
  });
  GEN_TIME_COST(time_cost, "{} producer={}", label, producer_size);
  waiter.WakeAll();

  int64_t res = 0;
  int64_t cnt = 0;
  const int64_t expect_res =
      producer_size * (producer_size - 1) / 2 * test_loop;
  BC bc;
  for (;;) {
    for (;;) {
      if (!mpmc_worker.TryGet([&](BenchNode &&v) { res += *v; })) {
        ++empty_cnt;
        rp(id, producer_size) { producer_notifiers[id].Wake(); }
        consumer_notifier.BlockedWait();
      } else {
        break;
      }
    }
    if (++cnt >= producer_size * test_loop) {
      break;
    }
    bc.run([&] {
      rp(id, producer_size) { producer_notifiers[id].Wake(); }
    });
  }

  dur = time_cost.Duration();
  RUNTIME_ASSERT(res == expect_res);
  RUNTIME_ASSERT(cnt == producer_size * test_loop);
}

static void bench_mpsc_spin_loop(std::string_view label, size_t producer_size,
                                 size_t test_loop, size_t producer_cap) {
  utils::MPSCWorker<BenchNode> mpsc_worker(producer_size, producer_cap);
  std::list<std::thread> producer_runners;
  utils::Waiter waiter;
  utils::CPUCacheLineAligned<std::atomic_uint64_t> tol_full_cnt{};
  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      auto full_cnt = 0;
      waiter.Wait();
      for (int i = 0; i < test_loop; ++i) {
        while (!mpsc_worker.Put(int(id), id)) {
          ++full_cnt;
          YIELD_CURRENT_THREAD(DEF_THREAD_YIELD_N);
        }
      }
      *tol_full_cnt += full_cnt;
    });
  }
  SCOPE_EXIT({
    for (auto &&t : producer_runners)
      t.join();
  });

  size_t empty_cnt = 0;
  utils::TimeCost::Clock::duration dur;
  SCOPE_EXIT({
    FMSGLN("    [full_cnt={}][empty_cnt={}]", tol_full_cnt->load(), empty_cnt);
    bench::ShowDurAvgAndOps(dur, producer_size * test_loop);
  });
  GEN_TIME_COST(time_cost, "{} producer={}", label, producer_size);
  waiter.WakeAll();

  std::atomic_int64_t res = 0;
  std::atomic_int64_t cnt = 0;
  const int64_t expect_res =
      producer_size * (producer_size - 1) / 2 * test_loop;
  for (;;) {
    auto x = mpsc_worker.Consume(
        [&](BenchNode &&v, size_t) {
          res += *v;
          // MSGLN(v);
        },
        [](size_t) {}, 1024 * 4);
    if (!x) {
      ++empty_cnt;
      YIELD_CURRENT_THREAD(DEF_THREAD_YIELD_N);
      continue;
    }
    cnt += x;
    if (cnt >= producer_size * test_loop) {
      break;
    }
  }

  dur = time_cost.Duration();

  RUNTIME_ASSERT(res == expect_res);
  RUNTIME_ASSERT(cnt == producer_size * test_loop);
}

template <typename NotifierType>
static void bench_mpsc_awake(std::string_view label, size_t producer_size,
                             size_t test_loop, size_t producer_cap) {
  NotifierType consumer_notifier;
  utils::MPSCQueueWithNotifer<utils::MPSCWorker<BenchNode>, NotifierType>
      mpsc_worker(producer_size, producer_cap);
  std::list<std::thread> producer_runners;
  utils::Waiter waiter;
  utils::CPUCacheLineAligned<std::atomic_uint64_t> tol_full_cnt{};
  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      size_t full_cnt = 0;
      BatchCounter<128> b;

      waiter.Wait();
      for (int i = 0; i < test_loop; ++i) {
        mpsc_worker.Put(int(id), id, [&]() {
          consumer_notifier.Wake();
          ++full_cnt;
        });
        b.run([&] { consumer_notifier.Wake(); });
      }
      consumer_notifier.Wake();
      *tol_full_cnt += full_cnt;
    });
  }
  SCOPE_EXIT({
    for (auto &&t : producer_runners)
      t.join();
  });

  size_t empty_cnt = 0;
  utils::TimeCost::Clock::duration dur;
  SCOPE_EXIT({
    FMSGLN("    [full_cnt={}][empty_cnt={}]", tol_full_cnt->load(), empty_cnt);
    bench::ShowDurAvgAndOps(dur, producer_size * test_loop);
  });
  GEN_TIME_COST(time_cost, "{} producer={}", label, producer_size);
  waiter.WakeAll();

  int64_t res = 0;
  int64_t cnt = 0;
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
          const auto &mpsc_worker_ = mpsc_worker;
          mpsc_worker_.producer_notifier(empty_producer_index).Wake();
          ++empty_cnt;
          //  MSGLN("wake producer", empty_producer_index);
        },
        4 * 1024, consumer_notifier);
    cnt += x;
    if (cnt >= producer_size * test_loop) {
      break;
    }
    mpsc_worker.WakeAllProducers();
  }

  dur = time_cost.Duration();

  RUNTIME_ASSERT(res == expect_res);
  RUNTIME_ASSERT(cnt == producer_size * test_loop);
}

template <typename NotifierType>
static void bench_spsc(std::string_view label, size_t producer_size,
                       size_t test_loop, size_t producer_cap) {
  using BC = BatchCounter<>;

  NotifierType consumer_notifier;
  utils::SPSCQueueWithNotifer<utils::SPSCQueue<BenchNode>, NotifierType>
      spsc_worker(producer_cap);
  std::optional<std::thread> producer_runner;
  utils::Waiter waiter;
  size_t tol_full_cnt{};
  test_loop *= producer_size;
  producer_runner.emplace([&]() {
    waiter.Wait();
    BC b;
    rp(i, test_loop) {
      spsc_worker.Put(static_cast<int>(i), [&]() {
        consumer_notifier.Wake();
        ++tol_full_cnt;
      });
      b.run([&] { consumer_notifier.Wake(); });
    }
    consumer_notifier.Wake();
  });

  SCOPE_EXIT(producer_runner->join());

  size_t empty_cnt = 0;
  utils::TimeCost::Clock::duration dur;
  SCOPE_EXIT({
    FMSGLN("    [full_cnt={}][empty_cnt={}]", tol_full_cnt, empty_cnt);
    bench::ShowDurAvgAndOps(dur, test_loop);
  });
  GEN_TIME_COST(time_cost, "{}", label);
  waiter.WakeAll();

  int64_t cnt = 0;
  int64_t res = 0;
  const int64_t expect_res = test_loop * (test_loop - 1) / 2;
  BC b;
  for (;;) {
    auto x = spsc_worker.Get(consumer_notifier, [&] {
      spsc_worker.producer_notifier().Wake();
      ++empty_cnt;
    });
    res += *x;
    if (++cnt >= test_loop) {
      break;
    }
    b.run([&] { spsc_worker.producer_notifier().Wake(); });
  }

  dur = time_cost.Duration();

  RUNTIME_ASSERT_EQ(cnt, test_loop);
  RUNTIME_ASSERT_EQ(res, expect_res);
}

template <typename SPSC>
static void bench_spsc_raw(std::string_view label, size_t producer_size,
                           size_t test_loop, size_t producer_cap) {

  SPSC spsc_queue(producer_cap);
  std::optional<std::thread> producer_runner;
  utils::Waiter waiter;
  size_t tol_full_cnt{};
  test_loop *= producer_size;
  producer_runner.emplace([&]() {
    waiter.Wait();
    rp(i, test_loop) {
      for (; !spsc_queue.Put(i);)
        ;
    }
  });

  SCOPE_EXIT(producer_runner->join());

  size_t empty_cnt = 0;
  utils::TimeCost::Clock::duration dur;
  SCOPE_EXIT({ bench::ShowDurAvgAndOps(dur, test_loop); });
  GEN_TIME_COST(time_cost, "{}", label);
  waiter.WakeAll();

  int64_t cnt = 0;
  for (;;) {
    for (; !spsc_queue.Get();)
      ;
    if (++cnt >= test_loop) {
      break;
    }
  }

  dur = time_cost.Duration();
}

static void bench_notifer(std::string_view label, size_t producer_size,
                          size_t test_loop, size_t producer_cap) {

  utils::AtomicNotifier consumer_notifier;
  utils::AtomicNotifier producer_notifier;
  std::optional<std::thread> producer_runner;
  utils::Waiter waiter;

  test_loop *= producer_size;
  producer_runner.emplace([&]() {
    waiter.Wait();

    rp(i, test_loop) {
      consumer_notifier.Wake();
      producer_notifier.BlockedWait();
    }
  });

  SCOPE_EXIT(producer_runner->join());

  utils::TimeCost::Clock::duration dur;
  SCOPE_EXIT({ bench::ShowDurAvgAndOps(dur, test_loop); });
  GEN_TIME_COST(time_cost, "{}", label);
  waiter.WakeAll();

  int64_t cnt = 0;
  const int64_t expect_res = test_loop * (test_loop - 1) / 2;
  for (;;) {
    consumer_notifier.BlockedWait();
    producer_notifier.Wake();
    if (++cnt >= test_loop) {
      break;
    }
  }

  dur = time_cost.Duration();
}

static void bench_mpsc_fastbin_alloc(std::string_view label,
                                     size_t producer_size, size_t test_loop,
                                     size_t producer_cap) {
  utils::MPSCQueueFastBinAlloc<BenchNode> mpsc_worker(
      producer_size, producer_cap * producer_size);
  std::list<std::thread> producer_runners;
  utils::Waiter waiter;
  utils::CPUCacheLineAligned<std::atomic_uint64_t> tol_full_cnt{};
  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      waiter.Wait();
      size_t full_cnt = 0;
      for (int i = 0; i < test_loop; ++i) {
        while (!mpsc_worker.TryPush(id, BenchElementType(id))) {
          ++full_cnt;
          YIELD_CURRENT_THREAD(DEF_THREAD_YIELD_N);
        }
      }
      *tol_full_cnt += full_cnt;
    });
  }
  SCOPE_EXIT({
    for (auto &&t : producer_runners)
      t.join();
  });

  size_t empty_cnt = 0;
  utils::TimeCost::Clock::duration dur;
  SCOPE_EXIT({
    FMSGLN("    [full_cnt={}][empty_cnt={}]", tol_full_cnt->load(), empty_cnt);
    bench::ShowDurAvgAndOps(dur, producer_size * test_loop);
  });
  GEN_TIME_COST(time_cost, "{} producer={}", label, producer_size);
  waiter.WakeAll();

  int64_t res = 0;
  int64_t cnt = 0;
  const int64_t expect_res =
      producer_size * (producer_size - 1) / 2 * test_loop;
  for (;;) {
    auto x = mpsc_worker.Consume([&](auto &&v) { res += *v; }, 1024 * 4);
    if (!x) {
      ++empty_cnt;
      YIELD_CURRENT_THREAD(DEF_THREAD_YIELD_N);
      continue;
    }
    cnt += x;
    if (cnt >= producer_size * test_loop) {
      break;
    }
  }

  dur = time_cost.Duration();

  RUNTIME_ASSERT(res == expect_res);
  RUNTIME_ASSERT(cnt == producer_size * test_loop);
}

static void bench_mpsc(int argc, char **argv) {
  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";
  size_t producer_size = argc > 1 ? std::stoi(argv[1]) : 8;

  using NormalSPSCQueue = NormalSPSCQueue<int>;
  using OptimizedSPSCQueue = utils::SPSCQueue<int>;

#define M(fn_name)                                                             \
  bench_##fn_name(("bench_" #fn_name), producer_size, 1024 * 1024 * 4, 1024)
  std::unordered_map<std::string, std::function<void()>> data = {
      {"ALL",
       [&] {
         M(mpsc_normal_stl);
         M(mpsc_awake<utils::AtomicNotifier>);
         M(mpsc_awake<utils::Notifier>);
         M(mpsc_spin_loop);
         // M(mpsc_fastbin_alloc);
         M(spsc<utils::AtomicNotifier>);
         M(spsc<utils::Notifier>);
         M(notifer);
         M(spsc_raw<OptimizedSPSCQueue>);
       }},
      {"STL", [&] { M(mpsc_normal_stl); }},
      {"AWAKE", [&] { M(mpsc_awake<utils::AtomicNotifier>); }},
      {"SPSC", [&] { M(spsc<utils::AtomicNotifier>); }},
      {"SPIN", [&] { M(mpsc_spin_loop); }},
      {"FASTBIN", [&] { M(mpsc_fastbin_alloc); }},
      {"NOTIFER", [&] { M(notifer); }},
      {"SPSC_RAW",
       [&] {
         M(spsc_raw<NormalSPSCQueue>);
         M(spsc_raw<OptimizedSPSCQueue>);
       }},
  };
  if (auto it = data.find(mode_str); it != data.end()) {
    it->second();
  } else {
    FMSGLN("Unknown mode `{}`, option:", mode_str);
    for (auto &&e : data) {
      FMSGLN("  {}", e.first);
    }
    return;
  }

#undef M
}

} // namespace

FUNC_FACTORY_REGISTER("MPSC", bench_mpsc)
