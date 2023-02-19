#include "utils/mpsc_base.hpp"

namespace bench {

struct Waiter : MutexLockWrap {
  std::condition_variable cv;
  bool start = false;

  void Wait() {
    auto lock = GenUniqueLock();
    while (!start)
      cv.wait(lock);
  }
  void WakeAll() {
    auto lock = GenLockGuard();
    start = true;
    cv.notify_all();
  }
};

template <typename T> struct MPMCNormal : MutexLockWrap {
  MPMCNormal(size_t producer_size, size_t producer_cap)
      : producer_cap_(producer_cap) {
    rp(i, producer_size)
        queues_.emplace_back(std::make_unique<Data>(producer_cap));
  }
  bool Put(T &&v, size_t index) {
    return queues_[index % queues_.size()]->emplace(std::move(v));
  }
  template <typename CB> bool Get(CB &&cb) {
    const auto ori_index = round_robin_index_;
    auto &&func = [&](size_t end) -> std::optional<T> {
      std::optional<T> res;
      for (; round_robin_index_ < end; ++round_robin_index_) {
        auto &queue = queues_[round_robin_index_];
        auto res = queue->pop();
        if (res)
          return res;
      }
      return std::nullopt;
    };
    auto res = func(queues_.size());
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
    Data(size_t cap) : cap_(cap) { inner_ = new T[cap_]; }
    ~Data() {
      rp(i, size_) inner_[(i + begin_) % cap_].~T();
      delete[] inner_;
    }
    T &operator[](size_t index) { return inner_[index]; }
    const T &operator[](size_t index) const { return inner_[index]; }

    size_t capacity() const { return cap_; }

    bool emplace(T &&val) {
      auto lock = GenLockGuard();
      if (size_ >= cap_)
        return false;
      new (inner_ + end_) T(std::move(val));
      end_ = (end_ + 1) % cap_;
      ++size_;
      return true;
    }

    std::optional<T> pop() {
      auto lock = GenLockGuard();
      if (0 == size_)
        return std::nullopt;
      auto index = begin_;
      begin_ = (begin_ + 1) % cap_;
      --size_;
      return std::move(inner_[index]);
    }

  private:
    const size_t cap_{};
    size_t begin_{};
    size_t end_{};
    size_t size_{};
    T *inner_{};
  };
  std::vector<std::unique_ptr<Data>> queues_;
  size_t round_robin_index_{};
};

void bench_mpsc_normal_stl(size_t producer_size, size_t test_loop,
                           size_t producer_cap) {
  MPMCNormal<int> mpmc_worker(producer_size, producer_cap);
  std::list<std::thread> producer_runners;
  Waiter waiter;
  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      waiter.Wait();
      for (int i = 0; i < test_loop; ++i) {
        while (!mpmc_worker.Put(id, id)) {
          std::this_thread::yield();
        }
      }
    });
  }
  waiter.WakeAll();

  TimeCost time_cost{__FUNCTION__};

  volatile int64_t res = 0, cnt = 0;
  const int64_t expect_res =
      producer_size * (producer_size - 1) / 2 * test_loop;
  for (;;) {
    auto x = mpmc_worker.Get([&](int v) { res += v; });
    cnt += x;
    if (cnt >= producer_size * test_loop) {
      break;
    }
    if (0 == x) {
      std::this_thread::yield();
    }
  }

  for (auto &&t : producer_runners)
    t.join();
  RUNTIME_ASSERT(res == expect_res);
  RUNTIME_ASSERT(cnt == producer_size * test_loop);
}

void bench_mpsc_spin_loop(size_t producer_size, size_t test_loop,
                          size_t producer_cap) {
  MPSCQueueWithNotifer<MPSCWorker<int>> mpsc_worker(producer_size,
                                                    producer_cap);
  std::list<std::thread> producer_runners;
  Waiter waiter;
  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      waiter.Wait();
      for (int i = 0; i < test_loop; ++i) {
        while (!mpsc_worker.TryPut(id, id)) {
          std::this_thread::yield();
        }
      }
    });
  }
  waiter.WakeAll();

  TimeCost time_cost{__FUNCTION__};

  volatile int64_t res = 0, cnt = 0;
  const int64_t expect_res =
      producer_size * (producer_size - 1) / 2 * test_loop;
  for (;;) {
    auto x = mpsc_worker.TryGet(
        [&](int v, size_t) {
          res += v;
          // MSGLN(v);
        },
        [](size_t) {}, 8192);
    if (x == 0) {
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

void bench_mpsc(size_t producer_size, size_t test_loop, size_t producer_cap) {
  MPSCQueueWithNotifer<MPSCWorker<int>> mpsc_worker(producer_size,
                                                    producer_cap);
  std::list<std::thread> producer_runners;
  Waiter waiter;
  std::atomic_uint64_t tol_full_cnt = 0;
  rp(id, producer_size) {
    producer_runners.emplace_back([&, id]() {
      waiter.Wait();
      size_t full_cnt = 0;
      for (int i = 0; i < test_loop; ++i) {
        auto res = mpsc_worker.Put(id, id, std::chrono::seconds{8192}, [&]() {
          mpsc_worker.WakeCustomer();
          ++full_cnt;
        });
        RUNTIME_ASSERT(res);
        mpsc_worker.WakeCustomer();
      }
      // at the end, must wake customer
      mpsc_worker.WakeCustomer();
      tol_full_cnt += full_cnt;
    });
  }
  waiter.WakeAll();

  size_t empty_cnt = 0;
  SCOPE_EXIT(
      { FMSGLN("    [full_cnt={}][empty_cnt={}]", tol_full_cnt, empty_cnt); });
  TimeCost time_cost{__FUNCTION__};

  volatile int64_t res = 0, cnt = 0;
  const int64_t expect_res =
      producer_size * (producer_size - 1) / 2 * test_loop;
  for (;;) {
    auto x = mpsc_worker.Get(
        [&](int v, [[maybe_unused]] size_t producer_index) {
          res += v;
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
} // namespace bench

int main() {
  bench::bench_mpsc_normal_stl(8, 1024 * 1024, 1024);
  bench::bench_mpsc(8, 1024 * 1024, 1024);
  bench::bench_mpsc_spin_loop(8, 1024 * 1024, 1024);
}