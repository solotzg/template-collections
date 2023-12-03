#include <tests/tests.h>
#include <utils/mpsc.hpp>

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

using TestNode = TestNode<int>;

static void _test_mpsc_with_notifer() {
  {
    ASSERT_EQ(TestNode::obj_cnt(), 0);
    size_t producer_size = 2;
    utils::Notifier notifier{};
    auto s = utils::MPSCQueueWithNotifer<
        utils::MPSCWorker<TestNode>>::NewMPSCQueueWithNotifer(producer_size, 3);
    auto test_loop = 1024;
    std::thread t1([&]() {
      for (int i = 0; i < test_loop; ++i) {
        ASSERT(s.Put(TestNode{i * 2}, 1, utils::Seconds{8192}, func_nothing2));
        notifier.Wake();
      }
    });
    std::thread t2([&]() {
      for (int i = 0; i < test_loop; ++i) {
        ASSERT(
            s.Put(TestNode{i * 2 + 1}, 0, utils::Seconds{8192}, func_nothing2));
        notifier.Wake();
      }
    });
    auto res = 0, cnt = 0;
    for (;;) {
      auto x = s.Get([&](TestNode &&t, size_t) { res += t.value(); },
                     [&](size_t empty_producer_index) {
                       s.WakeProducer(empty_producer_index);
                     },
                     utils::Seconds{8192}, 8192, notifier);
      ASSERT(x);
      cnt += x;
      if (res == (test_loop * 2 - 1) * (test_loop * 2) / 2) {
        ASSERT_EQ(cnt, test_loop * 2);
        break;
      }
    }
    t1.join();
    t2.join();
    ASSERT_EQ(TestNode::obj_cnt(), 0);
  }
  {
    utils::Notifier notifier;
    auto s = utils::MPSCQueueWithNotifer<
        utils::MPSCWorker<int>>::NewMPSCQueueWithNotifer(1, 1);
    ASSERT(s.Put(666, 0, utils::Seconds{8192}, func_nothing2));
    ASSERT(!s.Put(777, 0, utils::Milliseconds{5}, func_nothing2));
    ASSERT(s.Get([](auto &&c, size_t) { ASSERT_EQ(c, 666); }, func_nothing1,
                 utils::Seconds{8192}, 1, notifier));
    ASSERT(!s.Get([](auto &&c, size_t) { ASSERT_EQ(c, 666); }, func_nothing1,
                  utils::Milliseconds{5}, 1, notifier));
  }
}

static void _test_mpsc_queue() {
  utils::MPSCWorker<int> mpsc(2, 3);
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

static void _test_spin_lock() {
  utils::SpinLockWrap spin_lock;
  ASSERT(!spin_lock.IsLocked());
  spin_lock.RunWithTryLock([&](auto &&lock) {
    ASSERT(spin_lock.IsLocked());
    ASSERT(lock.locked());
    ASSERT(!lock.TryLock());
  });
  spin_lock.RunWithLock([&]() {
    ASSERT(spin_lock.IsLocked());
    spin_lock.RunWithTryLock([&](auto &&lock) {
      ASSERT(spin_lock.IsLocked());
      ASSERT(!lock.locked());
      ASSERT(!lock.TryLock());
    });
  });
  ASSERT(!spin_lock.IsLocked());
}

static void _test_mpsc_fast_bin_alloc() {
  constexpr size_t producer_size = 2;
  utils::MPSCQueueFastBinAlloc<TestNode> mpsc(producer_size, 10);
  {
    rp(i, 10) RUNTIME_ASSERT(mpsc.TryPush(0, i));
    RUNTIME_ASSERT(!mpsc.TryPush(0, std::numeric_limits<int>::max()));
    RUNTIME_ASSERT(mpsc.IsFull());

    size_t k = 0;
    mpsc.Consume(
        [&](TestNode &x) {
          RUNTIME_ASSERT_EQ(k, x.value());
          ++k;
        },
        5);
    RUNTIME_ASSERT(!mpsc.IsFull());
    RUNTIME_ASSERT(mpsc.size());
    RUNTIME_ASSERT_EQ(TestNode::obj_cnt(), 5);
    mpsc.Consume(
        [&](TestNode &x) {
          RUNTIME_ASSERT_EQ(k, x.value());
          ++k;
        },
        5);
    RUNTIME_ASSERT(!mpsc.size());
    RUNTIME_ASSERT(!mpsc.IsFull());
    mpsc.Consume([](auto &&) { PANIC(__PRETTY_FUNCTION__); });
    RUNTIME_ASSERT_EQ(TestNode::obj_cnt(), 0);
  }

  {
    std::list<std::thread> threads;
    constexpr size_t test_loop = 1024;
    utils::Waiter waiter;
    rp(id, producer_size) {
      threads.emplace_back([&, id] {
        waiter.Wait();
        for (int i = 0; i < test_loop; ++i) {
          for (; !mpsc.TryPush(id, i);) {
            std::this_thread::yield();
          }
        }
      });
    }
    waiter.WakeAll();
    size_t expect_res = threads.size() * (test_loop - 1) * test_loop / 2;
    size_t res_sum = 0;
    for (size_t total = 0;;) {
      auto cnt = mpsc.Consume([&](auto &&e) { res_sum += e.value(); }, 5);
      total += cnt;
      if (total == threads.size() * test_loop) {
        break;
      }
    }
    for (auto &&t : threads)
      t.join();
    mpsc.Consume([](auto &&) { PANIC(__PRETTY_FUNCTION__); });
    ASSERT_EQ(expect_res, res_sum);
    RUNTIME_ASSERT_EQ(TestNode::obj_cnt(), 0);
  }
}

static void _test_mpsc() {
  _test_mpsc_with_notifer();
  _test_mpsc_queue();
  _test_mpsc_fast_bin_alloc();
  _test_spin_lock();
}
} // namespace tests

FUNC_FACTORY_REGISTER("mpsc", tests::_test_mpsc);
