#pragma once

#include "spsc_queue.hpp"

/*
enum class Priority : size_t { High = 0, Mid, Low, _MAX };

static std::string_view PriorityName(Priority p) {
  static constexpr std::string_view names[] = {
      "High",
      "Mid",
      "Low",
  };
  static_assert(std::size(names) == size_t(Priority::_MAX));
  return names[size_t(p)];
}

struct MPSCWorkerConfig {
  static constexpr size_t Cnt = static_cast<size_t>(Priority::_MAX);
  const std::array<size_t, Cnt> queue_size_;
  const std::array<size_t, Cnt> queue_cap_;
};

struct size_t : std::array<size_t, MPSCWorkerConfig::Cnt> {};

static std::ostream &operator<<(std::ostream &ostr, const size_t &node) {
  ostr << fmt::format("[{}]", fmt::join(node, ","));
  return ostr;
}
*/

template <typename T, typename Allocator = std::allocator<T>,
          typename AllocatorSPSC = std::allocator<SPSCQueue<T, Allocator>>>
struct MPSCWorker {
  using Element = T;
  using SPSCQueueType = SPSCQueue<T, Allocator>;
  using String = std::string;

  explicit MPSCWorker(size_t producer_size, size_t queue_cap,
                      const Allocator &allocator = Allocator(),
                      const AllocatorSPSC &spsc_allocator = AllocatorSPSC())
      : producer_size_(producer_size), spsc_allocator_(spsc_allocator) {
    {
      spsc_queues_ = std::allocator_traits<AllocatorSPSC>::allocate(
          spsc_allocator_, producer_size_);
    }
    for (size_t pos = 0; pos != producer_size_; ++pos) {
      new (spsc_queues_ + pos) SPSCQueueType(queue_cap, allocator);
    }
  }

  size_t producer_size() const { return producer_size_; }

  ~MPSCWorker() {
    for (size_t i = 0; i < producer_size_; ++i) {
      spsc_queues_[i].~SPSCQueueType();
    }
    std::allocator_traits<AllocatorSPSC>::deallocate(
        spsc_allocator_, spsc_queues_, producer_size_);
  }

  // unsafe
  // If `pos_hint` is same, `data` will be put into same FIFO channel.
  bool Put(T &&data, size_t pos_hint) {
    size_t pos = pos_hint % producer_size_;
    return spsc_queues_[pos].GenProducer().Put(std::move(data));
  }
  // unsafe
  // `index` must be LT queue size
  bool PutAt(T &&data, size_t index) {
    ASSERT_LT(index, producer_size_);
    return spsc_queues_[index].GenProducer().Put(std::move(data));
  }

  String Show() const {
    fmt::memory_buffer out{};
    Format(out);
    return fmt::to_string(out);
  }
  void Format(fmt::memory_buffer &out) const {
    SCOPE_EXIT_FMT_SURROUND(out, "{", "}");
    FMT_APPEND(out, "size:{}", producer_size_);
    FMT_APPEND(out, ", data:");
    SCOPE_EXIT_FMT_SURROUND(out, "{", "}");
    {
      for (size_t i = 0; i < producer_size_; ++i) {
        FMT_IF_APPEND(out, i, ", ");
        FMT_APPEND(out, "queue_{}:", i);
        SCOPE_EXIT_FMT_SURROUND(out, "{", "}");
        auto &spsc = spsc_queues_[i];
        spsc.Format(out);
      }
    }
  }

  struct MPSCWorkerCustomer {
    template <typename CallBack, typename CallBackWhenEmpty>
    size_t Consume(CallBack &&cb, CallBackWhenEmpty &&cb_empty,
                   const size_t max_consume_cnt = 8) {
      return worker_.Consume(cb, cb_empty, max_consume_cnt);
    }

    MPSCWorkerCustomer(MPSCWorker &worker) : worker_(worker) {}

  private:
    friend struct MPSCWorker;
    MPSCWorkerCustomer(const MPSCWorkerCustomer &) = delete;
    MPSCWorker &worker_;
  };

  MPSCWorkerCustomer GenCustomer() { return MPSCWorkerCustomer{*this}; }

protected:
  // unsafe
  template <typename CallBack, typename CallBackWhenEmpty>
  size_t Consume(CallBack &&cb, CallBackWhenEmpty &&cb_empty,
                 const size_t max_consume_cnt) {
    size_t consume_cnt{};

    for (;;) {
      bool any_consumed = false;
      const auto ori_queue_index = queue_round_robin_index_;

      auto &&func_loop = [&](const size_t end) {
        for (; queue_round_robin_index_ < end && consume_cnt < max_consume_cnt;
             ++queue_round_robin_index_) {
          auto &spsc = spsc_queues_[queue_round_robin_index_];
          if (auto t = spsc.GenCustomer().Get(); t) {
            cb(std::move(*t), queue_round_robin_index_);
            consume_cnt++;
            any_consumed = true;
          } else {
            cb_empty(queue_round_robin_index_);
          }
        }
      };

      {
        // round robin start ~ end
        func_loop(producer_size_);
      }
      if (consume_cnt < max_consume_cnt) {
        // 0 ~ round robin start
        queue_round_robin_index_ = 0;
        func_loop(ori_queue_index);
      }

      if (!any_consumed) {
        break;
      }
    }

    return consume_cnt;
  }

private:
  MPSCWorker(const MPSCWorker &) = delete;

private:
  const size_t producer_size_;
  AllocatorSPSC spsc_allocator_;
  SPSCQueueType *spsc_queues_;
  // writable
  alignas(BasicConfig::CPU_CACHE_LINE_SIZE) size_t queue_round_robin_index_{};
};

template <typename MPSC> struct MPSCQueueWithNotifer {
  using Element = typename MPSC::Element;

  template <typename CB>
  bool Put(Element &&e, size_t pos, const std::chrono::milliseconds &timeout,
           CB &&cb_when_full) {
    if (auto res = TryPut(std::forward<Element>(e), pos); res) {
      return res;
    }
    auto time_point = std::chrono::steady_clock::now() + timeout;
    return Put(std::forward<Element>(e), pos, time_point,
               std::move(cb_when_full));
  }
  template <typename CB>
  bool Put(Element &&e, size_t pos,
           const std::chrono::steady_clock::time_point &time_point,
           CB &&cb_when_full) {
    for (;;) {
      if (auto res = TryPut(std::forward<Element>(e), pos); res) {
        return res;
      } else {
        cb_when_full();
      }
      switch (producer_notifier_[pos].BlockedWaitUtil(time_point)) {
      case AsyncNotifier::Status::Timeout:
        return false;
      case AsyncNotifier::Status::Normal:
        break;
      }
    }
  }

  bool TryPut(Element &&e, size_t pos) {
    return mpsc_queue_.PutAt(std::forward<Element>(e), pos);
  }

  template <typename CallBack, typename CallBackWhenEmpty>
  size_t Get(CallBack &&cb, CallBackWhenEmpty &&cb_empty,
             const std::chrono::milliseconds &timeout,
             const size_t max_consume_cnt) {
    if (auto res = TryGet(cb, cb_empty, max_consume_cnt); res) {
      return res;
    }
    auto time_point = std::chrono::steady_clock::now() + timeout;
    return Get(cb, cb_empty, time_point, max_consume_cnt);
  }

  template <typename CallBack, typename CallBackWhenEmpty>
  size_t Get(CallBack &&cb, CallBackWhenEmpty &&cb_empty,
             const std::chrono::steady_clock::time_point &time_point,
             const size_t max_consume_cnt) {
    for (;;) {
      if (auto res = TryGet(cb, cb_empty, max_consume_cnt); res) {
        return res;
      }
      switch (customer_notifier_.BlockedWaitUtil(time_point)) {
      case AsyncNotifier::Status::Timeout:
        return 0;
      case AsyncNotifier::Status::Normal:
        break;
      }
    }
  }

  template <typename CallBack, typename CallBackWhenEmpty>
  size_t TryGet(CallBack &&cb, CallBackWhenEmpty &&cb_empty,
                const size_t max_consume_cnt) {
    return mpsc_queue_.GenCustomer().Consume(cb, cb_empty, max_consume_cnt);
  }

  void WakeProducer(size_t pos) const { producer_notifier_[pos].Wake(); }
  void WakeCustomer() const { customer_notifier_.Wake(); }

  template <typename... Args>
  MPSCQueueWithNotifer(Args &&...args)
      : mpsc_queue_(std::forward<Args>(args)...) {
    producer_notifier_ = new Notifier[mpsc_queue_.producer_size()];
  }

  ~MPSCQueueWithNotifer() { delete[] producer_notifier_; }

  typename MPSC::String Show() const { return mpsc_queue_.Show(); }

private:
  Notifier *producer_notifier_;
  MPSC mpsc_queue_;
  Notifier customer_notifier_;
};

#ifndef NDEBUG

template <typename T>
struct fmt::formatter<std::vector<T>> : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::vector<T> &v, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(fmt::join(v, ","), ctx);
  }
};

namespace tests {

static auto &&func_nothing1 = [](size_t) {};
static auto &&func_nothing2 = []() {};

static void _test_mpsc_with_notifer() {
  {
    ASSERT_EQ(TestNode::test_node_cnt(), 0);
    auto producer_size = 2;
    MPSCQueueWithNotifer<MPSCWorker<TestNode>> s(producer_size, 3);
    auto test_loop = 1024;
    std::thread t1([&]() {
      for (int i = 0; i < test_loop; ++i) {
        ASSERT(s.Put(TestNode{i * 2}, 1, std::chrono::seconds{8192},
                     func_nothing2));
        s.WakeCustomer();
      }
    });
    std::thread t2([&]() {
      for (int i = 0; i < test_loop; ++i) {
        ASSERT(s.Put(TestNode{i * 2 + 1}, 0, std::chrono::seconds{8192},
                     func_nothing2));
        s.WakeCustomer();
      }
    });
    auto res = 0, cnt = 0;
    for (;;) {
      auto x = s.Get([&](TestNode &&t, size_t) { res += t.v; },
                     [&](size_t empty_producer_index) {
                       s.WakeProducer(empty_producer_index);
                     },
                     std::chrono::seconds{8192}, 8192);
      ASSERT(x);
      cnt += x;
      if (res == (test_loop * 2 - 1) * (test_loop * 2) / 2) {
        ASSERT_EQ(cnt, test_loop * 2);
        break;
      }
    }
    t1.join();
    t2.join();
    ASSERT_EQ(TestNode::test_node_cnt(), 0);
  }
  {
    MPSCQueueWithNotifer<MPSCWorker<int>> s(1, 1);
    ASSERT(s.Put(666, 0, std::chrono::seconds{8192}, func_nothing2));
    ASSERT(!s.Put(777, 0, std::chrono::milliseconds{5}, func_nothing2));
    ASSERT(s.Get([](auto &&c, size_t) { ASSERT_EQ(c, 666); }, func_nothing1,
                 std::chrono::seconds{8192}, 1));
    ASSERT(!s.Get([](auto &&c, size_t) { ASSERT_EQ(c, 666); }, func_nothing1,
                  std::chrono::milliseconds{5}, 1));
  }
}

static void _test_mpsc_queue() {
  MPSCWorker<int> mpsc(2, 3);
  //
  ASSERT(mpsc.Put(1, 0));
  ASSERT(mpsc.Put(2, 1));
  ASSERT(mpsc.Put(3, 2));
  ASSERT(mpsc.Put(4, 3));
  ASSERT(mpsc.Put(5, 4));
  ASSERT(mpsc.Put(6, 5));
  ASSERT(!mpsc.Put(7, 1024));
  ASSERT_EQ(
      mpsc.Show(),
      "{size:2, data:{queue_0:{`1`, `3`, `5`}, queue_1:{`2`, `4`, `6`}}}");
  {
    std::vector<int> data{};
    auto r = mpsc.GenCustomer().Consume(
        [&](int x, size_t) { data.emplace_back(x); }, func_nothing1, 3);
    ASSERT_EQ(r, 3);
    ASSERT(data == std::vector<int>({1, 2, 3}));
    ASSERT_EQ(mpsc.Show(),
              "{size:2, data:{queue_0:{`5`}, queue_1:{`4`, `6`}}}");
  }
  {
    std::vector<int> data;
    auto r = mpsc.GenCustomer().Consume(
        [&](int x, size_t) { data.emplace_back(x); }, func_nothing1, 2);
    ASSERT_EQ(r, 2);
    ASSERT(data == (std::vector{4, 5}));
    ASSERT_EQ(mpsc.Show(), "{size:2, data:{queue_0:{}, queue_1:{`6`}}}");
  }

  {
    std::set<int> data;
    auto r = mpsc.GenCustomer().Consume([&](int x, size_t) { data.emplace(x); },
                                        func_nothing1,
                                        std ::numeric_limits<size_t>::max());
    ASSERT_EQ(r, 1);
    ASSERT(data == std::set<int>{6});
  }
}

static void _test_mpsc() {
  _test_mpsc_with_notifer();
  _test_mpsc_queue();
}

} // namespace tests
#endif