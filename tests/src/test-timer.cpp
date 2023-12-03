
#include <tests/tests.h>
#include <utils/timer.hpp>

namespace tests {

using Timer = utils::Timer;

struct TimerTest : Timer {
  using Timer::Timer;

  static void test() {
    std::atomic_bool flag = false;
    std::atomic_bool cancel_test = false;
    TimerTest timer;
    Timer::Delay delay{0};
    timer.Schedule(
        [&](bool stop) {
          assert(!stop);
          flag = true;
        },
        delay);
    auto cancel_case = timer.Schedule(
        [&](bool stop) {
          assert(!stop);
          cancel_test = true;
        },
        delay);
    assert(flag == false);
    std::this_thread::sleep_for(utils::Milliseconds{1});
    assert(flag == false);
    assert(cancel_case->Cancel());
    assert(!cancel_case->Cancel());
    assert(timer.RunOneRound() == 2);
    assert(!cancel_test);
    assert(flag == true);
  }
};

struct TestAsyncTime : utils::chrono::AsyncClock {
  static void test2() {
    TestAsyncTime async_time{};
    auto timer = utils::DelegateWheelTimer::DelegateRun(async_time, 1, 1);
    {
      std::atomic_bool flag{};
      timer->Schedule(
          [&](bool stop) {
            ASSERT(!stop);
            flag = true;
          },
          utils::Milliseconds{1});
      ASSERT(!flag);
      async_time.RunTasks();
      ASSERT(!flag);
      async_time.clock_.data_->steady_time_point_millisec += 1;
      async_time.RunTasks();
      ASSERT(flag);
    }
    {
      async_time.clock_.data_->steady_time_point_millisec += 1;
      std::atomic_bool flag{};
      timer->Schedule(
          [&](bool stop) {
            ASSERT(!stop);
            flag = true;
          },
          utils::Milliseconds{0});
      ASSERT(!flag);
      async_time.RunTasks();
      ASSERT(flag);
    }
    {
      async_time.clock_.data_->steady_time_point_millisec += 1;
      auto task = timer->Schedule([&](bool) { std::terminate(); },
                                  utils::Milliseconds{0});
      ASSERT(task->Cancel());
      async_time.RunTasks();
      ASSERT(!task->Cancel());
      ASSERT(!task->Run(true));
    }
  }

  static void test1() {
    TestAsyncTime async_time{utils::MaxDuration};
    ASSERT_NE(async_time.clock().steady_time_point().time_since_epoch().count(),
              0);
    ASSERT_NE(async_time.clock().system_time_point().time_since_epoch().count(),
              0);
    {
      auto c1 = async_time.clock().steady_time_point();
      auto c2 = async_time.clock().system_time_point();
      ASSERT(async_time.AsyncRun());
      ASSERT(!async_time.AsyncRun());
      for (; c1 == async_time.clock().steady_time_point() ||
             c2 == async_time.clock().system_time_point();) {
        std::this_thread::yield();
      }
    }

    {
      auto c1 = async_time.clock().steady_time_point();
      auto c2 = async_time.clock().system_time_point();

      ASSERT_EQ(
          async_time.clock().steady_time_point().time_since_epoch().count(),
          c1.time_since_epoch().count());
      ASSERT_EQ(async_time.clock().system_time_point(), c2);
      async_time.RunOneRound();
      ASSERT_GT(
          async_time.clock().steady_time_point().time_since_epoch().count(),
          c1.time_since_epoch().count());
      ASSERT_GT(async_time.clock().system_time_point(), c2);
    }

    {
      auto c1 = async_time.clock().steady_time_point();
      auto c2 = async_time.clock().system_time_point();
      async_time.notifier()->Wake();
      for (; c1 == async_time.clock().steady_time_point() ||
             c2 == async_time.clock().system_time_point();) {
        std::this_thread::yield();
      }
    }
  }
};

static void _test_timer() {
  {
    utils::FastBin<uint8_t> fast_bin;
    auto *p = fast_bin.Alloc();
    assert((size_t)(p) % 8 == 0);
    fast_bin.Dealloc(p);
    auto *q = fast_bin.Alloc();
    assert(p == q);
  }
  { TimerTest::test(); }
  {
    Timer timer;

    timer.AsyncRun();
    auto old = Timer::Clock::now();
    auto check = old;
    utils::Notifier notifier;
    Timer::Delay delay(1);
    timer.Schedule(
        [&](bool stop) {
          check = Timer::Clock::now();
          notifier.Wake();
          assert(!stop);
        },
        delay);
    notifier.BlockedWait();
    assert((check - old) >= delay);
    timer.Schedule([](bool stop) { assert(stop); },
                   Timer::Delay(utils::MaxDuration));
    timer.StopAndWait();
    timer.Schedule([](bool stop) { assert(stop); },
                   Timer::Delay(utils::MaxDuration));
  }
  {
    Timer timer;

    timer.AsyncRun();
    struct Test : utils::MutexLockWrap {
      void Add(int x) {
        RunWithMutexLock([&] { res.emplace_back(x); });
      }
      std::vector<int> res;
    } tests;
    const int test_cnt = 10;
    std::atomic_int cnt = test_cnt;
    utils::Notifier notifier;
    utils::Waiter waiter;
    timer.Schedule([&](bool stop) { waiter.Wait(); }, Timer::Delay(0));
    for (int i = 0; i < test_cnt; ++i) {
      timer.Schedule(
          [&, i](bool stop) {
            assert(!stop);
            tests.Add(i);
            if (--cnt == 0) {
              notifier.Wake();
            }
          },
          Timer::Delay(test_cnt - i));
    }
    waiter.WakeAll();
    notifier.BlockedWait();
    for (int i = 0; i < test_cnt; ++i) {
      assert(tests.res[i] == (test_cnt - 1 - i));
    }
    timer.StopAndWait();
  }
  {
    TestAsyncTime::test1();
    TestAsyncTime::test2();
  }
}
} // namespace tests

FUNC_FACTORY_REGISTER("timer", tests::_test_timer);
