#pragma once

#include <utils/async.hpp>

namespace utils {

struct UniqAsyncLog {

  template <size_t maxn> struct BufferCache : utils::MutexLockWrap {
    template <typename F>
    void Write(char *src, size_t n, const utils::SysMilliseconds time_point,
               F &&write_func) {
      auto &&_ = GenLockGuard();

      if (size() + n >= maxn) {
        write_func(data(), size());
        Clear();
      }

      if unlikely (n > maxn) {
        n = maxn;
      }

      if (time_point > last_time_point_) {
        last_time_point_ = time_point;
        std::memcpy(last_time_point_str_.data(), src, LOG_TIMEPOINT_SIZE);
        WriteToEnd(src, n);
      } else if (time_point < last_time_point_) {
        WriteToEnd(last_time_point_str_.data(), LOG_TIMEPOINT_SIZE);
        WriteToEnd(src + LOG_TIMEPOINT_SIZE, n - LOG_TIMEPOINT_SIZE);
      } else {
        WriteToEnd(src, n);
      }
    }

    template <typename F> void Flush(F &&write_func) {
      auto &&_ = GenLockGuard();

      if (size() == 0)
        return;
      write_func(data(), size());
      Clear();
    }

    const char *data() const { return data_.data(); }
    size_t size() const { return size_; }

    BufferCache() = default;

  private:
    void Clear() { size_ = 0; }
    char *end() { return data_.data() + size(); }
    void WriteToEnd(const char *src, size_t n) {
      std::memcpy(end(), src, n);
      size_ += n;
    }

  private:
    std::array<char, maxn> data_;
    std::array<char, LOG_TIMEPOINT_SIZE> last_time_point_str_;
    utils::SysMilliseconds last_time_point_{};
    size_t size_{};
  };

  template <typename... Args> void Flush(Args &&...args) {
    buffer_cache_.Flush(std::forward<Args>(args)...);
  }

  template <typename... Args> void Write(Args &&...args) {
    buffer_cache_.Write(std::forward<Args>(args)...);
  }

  static constexpr size_t kDefaultBuffCacheSize = 16 * 1024;

  UniqAsyncLog() = default;

private:
  mutable BufferCache<kDefaultBuffCacheSize> buffer_cache_;
}; // namespace utils

struct AsyncLogger {
  using Log = UniqAsyncLog;
  using Self = AsyncLogger;
  using TimePoint = typename chrono::AsyncSystemClockMs::time_point;
  static constexpr U32 kDefaultFlushDurMs = 10;

  AsyncLogger(Log &logger) : logger_(logger) {}

  static Self &GlobalStdout() {
    static auto async_logger = GlobalStdoutImpl();

    auto &&timer = utils::WheelTimerRunner::GlobalInstance();
    Run(false, &async_logger, &timer, kDefaultFlushDurMs);

    return async_logger;
  }

  void Write(char *src, size_t n, const utils::SysMilliseconds time_point) {
    logger_.Write(src, n, time_point, [](const char *p, size_t n) {
      utils::RunWithGlobalStdoutLock([&] { std::fwrite(p, 1, n, stdout); });
    });
  }

  void Flush() {
    logger_.Flush([](const char *p, size_t n) {
      utils::RunWithGlobalStdoutLock([&] { std::fwrite(p, 1, n, stdout); });
    });
  }

  TimePoint Now() const { return system_clock_.now(); }

private:
  static void Run(bool is_stop, Self *async_logger,
                  utils::WheelTimerRunner *timer, U32 delay) {
    async_logger->Flush();
    if (is_stop)
      return;
    timer->Schedule(
        [async_logger, timer, delay](bool is_stop) {
          Run(is_stop, async_logger, timer, delay);
        },
        delay);
  }

  static Self GlobalStdoutImpl() {
    static auto &&async_logger = Log{};

    return Self(async_logger);
  }

private:
  Log &logger_;
  chrono::AsyncSystemClockMs system_clock_{
      chrono::AsyncClock::GlobalInstance().clock()};
};

} // namespace utils
