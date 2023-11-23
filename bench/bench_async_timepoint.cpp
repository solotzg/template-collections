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

  // T &operator[](size_t n) { return at(n); }

  void Pushlish(T &&src) {
    auto &&old = lock_.RunWithLock([this, &src] {
      return std::exchange(at(published_index_), std::move(src));
    });
    auto v = published_index_++;
    if (!v)
      published_index_.notify_one();
  }

  void WaitAvailable() const { published_index_.wait(0); }

private:
  const T &at(size_t n) const { return data_[n % N]; }
  T &at(size_t n) { return data_[n % N]; }

private:
  std::vector<T> data_;
  std::atomic_int64_t published_index_{};
  utils::SpinLockWrap lock_;
};

NO_INLINE void run_stl(char *p) {
  utils::SerializeTimepoint(p, utils::SystemClock::now());
}
NO_INLINE void run_async(char *RESTRICT p, const char *RESTRICT src) {
  std::memcpy(p, src, utils::kLogTimePointSize);
}

static void bench_async_timepoint(int argc, char **argv) {
#ifdef NDEBUG
  const size_t n = 1e7;
#else
  const size_t n = 100;
#endif
  std::array<char, utils::kLogTimePointSize> buff;

  auto fn_bench_stl = [&]() {
    utils::TimeCost tc("STL", false);
    rp(i, n) {
      run_stl(buff.data());
      DEBUG_MSGLN(std::string_view{buff.data(), buff.size()});
      DEBUG_SCOPE({ std::this_thread::sleep_for(utils::Milliseconds{1}); });
    }
    tc.Show();
    FMSGLN("OPS {:.3f}", double(n) / tc.DurationSec());
  };

  auto fn_bench_async = [&]() {
    FastBin allocator;
    RingBuffer<utils::SharedObjectPtr<FastBin>, 1024> rang_data;
    utils::chrono::AsyncClock aclock(utils::Milliseconds{1}, true);

    aclock.AddTask([&allocator, &aclock, &rang_data] {
      auto &&p = utils::SharedObjectPtr<FastBin>::New(allocator);
      utils::SerializeTimepoint(p->buff_.data(),
                                aclock.clock().system_time_point());
      rang_data.Pushlish(std::move(p));
    });

    {
      SHOW_TIME_COST("RingBuffer.WaitAvailable");
      rang_data.WaitAvailable();
    }

    utils::TimeCost tc("ASYNC", 0);
    rp(i, n) {
      auto ptr = rang_data.GetLast();
      run_async(buff.data(), ptr->buff_.data());
      DEBUG_MSGLN(std::string_view{buff.data(), buff.size()});
      DEBUG_SCOPE({ std::this_thread::sleep_for(utils::Milliseconds{1}); });
    }
    tc.Show();
    FMSGLN("OPS {:.3f}", double(n) / tc.DurationSec());
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