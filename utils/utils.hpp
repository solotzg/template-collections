#pragma once

#include "head_define.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>

struct BasicConfig {
  static constexpr size_t CPU_CACHE_LINE_SIZE = 64;
};

class MutexLockWrap {
public:
  using Mutex = std::mutex;

protected:
  std::lock_guard<Mutex> GenLockGuard() const {
    return std::lock_guard(mutex());
  }

  std::unique_lock<Mutex> TryToLock() const {
    return std::unique_lock(mutex(), std::try_to_lock);
  }

  std::unique_lock<Mutex> GenUniqueLock() const {
    return std::unique_lock(mutex());
  }

  Mutex &mutex() const { return mutex_; }

private:
  mutable Mutex mutex_;
};

class SharedMutexLockWrap {
public:
  using Mutex = std::shared_mutex;

protected:
  std::shared_lock<Mutex> GenSharedLock() const {
    return std::shared_lock(mutex());
  }

  std::unique_lock<Mutex> GenUniqueLock() const {
    return std::unique_lock(mutex());
  }

  Mutex &mutex() const { return mutex_; }

private:
  mutable Mutex mutex_;
};

struct AsyncNotifier {
  enum class Status {
    Timeout,
    Normal,
  };
  // NOT thread safe
  virtual Status BlockedWaitFor(const std::chrono::milliseconds &) {
    return AsyncNotifier::Status::Timeout;
  }
  virtual Status
  BlockedWaitUtil(const std::chrono::steady_clock::time_point &) {
    return AsyncNotifier::Status::Timeout;
  }
  // thread safe
  virtual bool IsAwake() const = 0;
  virtual void Wake() const = 0;
  virtual ~AsyncNotifier() = default;
};

struct Notifier final : AsyncNotifier, MutexLockWrap {
  Status BlockedWaitFor(const std::chrono::milliseconds &timeout) override {
    return BlockedWaitUtil(std::chrono::steady_clock::now() + timeout);
  }

  Status BlockedWaitUtil(
      const std::chrono::steady_clock::time_point &time_point) override {
    // if flag from false to false, wait for notification.
    // if flag from true to false, do nothing.
    auto res = AsyncNotifier::Status::Normal;
    if (!is_awake_.exchange(false, std::memory_order_acq_rel)) {
      {
        auto lock = GenUniqueLock();
        if (!is_awake_.load(std::memory_order_acquire)) {
          if (cv_.wait_until(lock, time_point) == std::cv_status::timeout)
            res = AsyncNotifier::Status::Timeout;
        }
      }
      is_awake_.store(false, std::memory_order_release);
    }
    return res;
  }

  void Wake() const override {
    // if flag from false -> true, then wake up.
    // if flag from true -> true, do nothing.
    if (IsAwake()) {
      return;
    }
    if (!is_awake_.exchange(true, std::memory_order_acq_rel)) {
      // wake up notifier
      auto _ = GenLockGuard();
      cv_.notify_one();
    }
  }

  bool IsAwake() const override {
    return is_awake_.load(std::memory_order_acquire);
  }

  virtual ~Notifier() = default;

private:
  mutable std::condition_variable cv_;
  // multi notifiers single receiver model. use another flag to avoid waiting
  // endlessly.
  alignas(BasicConfig::CPU_CACHE_LINE_SIZE) mutable std::atomic_bool is_awake_{
      false};
};

static uint32_t NextPow2(uint32_t v) {
  ASSERT_GT(v, 0);
  ASSERT_LE(v, 1u << 31);
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}
