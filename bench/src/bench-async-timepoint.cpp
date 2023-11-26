#include "bench/bench.h"
#include "utils/timer.hpp"
#include "utils/utils.h"

#ifndef NDEBUG
constexpr bool kCheckConsistency = true;
#else
constexpr bool kCheckConsistency = false;
#endif

namespace {

struct TestNode : utils::ConsistencyChecker<kCheckConsistency>,
                  utils::noncopyable {
  std::array<char, utils::kLogTimePointSize> buff_;
};

struct FastBin : utils::FastBin<utils::SharedObject<TestNode>>,
                 utils::SpinLockWrap {
  using Base = utils::FastBin<utils::SharedObject<TestNode>>;
  using ObjectType = Base::ObjectType;

  void *Alloc() {
    return RunWithLock([this] { return Base::Alloc(); });
  }
  void Dealloc(void *ptr) {
    return RunWithLock([ptr, this] { return Base::Dealloc(ptr); });
  }
};

template <typename T, size_t N> struct RingBuffer : utils::noncopyable {

  RingBuffer() : data_(N, nullptr) {}

  T GetLast() {
    return lock_.RunWithLock([this] { return at(published_index_ - 1); });
  }

  bool empty() const { return published_index_ == 0; }

  void Pushlish(T &&src) {
    lock_.RunWithLock([this, &src] {
      return std::exchange(at(published_index_), std::move(src));
    });
    published_index_++;
  }

private:
  const T &at(size_t n) const { return data_[n % N]; }
  T &at(size_t n) { return data_[n % N]; }

private:
  std::vector<T> data_;
  std::atomic_int64_t published_index_{};
  utils::SpinLockWrap lock_;
};

NO_INLINE void run_stl(char *p, utils::SysSeconds &last_update_time_sec) {
  utils::SerializeTimepoint(p, last_update_time_sec, utils::SystemClock::now());
}
NO_INLINE void run_async(char *RESTRICT p, const char *RESTRICT src) {
  std::memcpy(p, src, utils::kLogTimePointSize);
}

static void bench_async_timepoint(int argc, char **argv) {
#ifdef NDEBUG
  const size_t n = 1e7;
#else
  const size_t n = 20;
#endif
  std::array<char, utils::kLogTimePointSize> buff;
  utils::SysSeconds last_update_time_sec{};

  auto fn_bench_stl = [&]() {
    utils::TimeCost tc("STL", false);
    rp(i, n) {
      run_stl(buff.data(), last_update_time_sec);
      DEBUG_MSGLN(std::string_view{buff.data(), buff.size()});
      DEBUG_SCOPE({ std::this_thread::sleep_for(utils::Milliseconds{1}); });
    }
    auto &&dur = tc.Duration();
    tc.Show();
    bench::ShowDurAvgAndOps(dur, n);
  };

  auto fn_bench_async = [&]() {
    FastBin allocator;
    RingBuffer<utils::SharedObjectPtr<FastBin>, 1024> rang_data;
    utils::chrono::AsyncClock aclock(utils::Milliseconds{1}, true);

    utils::chrono::AsyncClock::Task task = [&allocator, &aclock, &rang_data,
                                            &last_update_time_sec, &buff] {
      auto &&p = utils::SharedObjectPtr<FastBin>::New(allocator);
      {
        utils::SerializeTimepoint(buff.data(), last_update_time_sec,
                                  aclock.clock().system_time_point());
        p->buff_ = buff;
      }
      rang_data.Pushlish(std::move(p));
    };

    {
      task();
      RUNTIME_ASSERT(!rang_data.empty());
      aclock.AddTask(std::move(task));
    }

    utils::TimeCost tc("ASYNC", 0);
    rp(i, n) {
      auto ptr = rang_data.GetLast();
      run_async(buff.data(), ptr->buff_.data());
      DEBUG_MSGLN(std::string_view{buff.data(), buff.size()});
      DEBUG_SCOPE({ std::this_thread::sleep_for(utils::Milliseconds{1}); });
    }
    auto &&dur = tc.Duration();
    tc.Show();
    bench::ShowDurAvgAndOps(dur, n);
  };

  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";

  if (mode_str == "ALL") {
    fn_bench_stl();
    fn_bench_async();
  } else if (mode_str == "ASYNC") {
    fn_bench_async();
  } else if (mode_str == "STL") {
    fn_bench_stl();
  }
}

FUNC_FACTORY_REGISTER("ASYNC_TIMEPOINT", bench_async_timepoint)

} // namespace