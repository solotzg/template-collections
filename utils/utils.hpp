#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>

class MutexLockWrap {
public:
  std::lock_guard<std::mutex> genLockGuard() const {
    return std::lock_guard(mutex_);
  }

  std::unique_lock<std::mutex> tryToLock() const {
    return std::unique_lock(mutex_, std::try_to_lock);
  }

  std::unique_lock<std::mutex> genUniqueLock() const {
    return std::unique_lock(mutex_);
  }

private:
  mutable std::mutex mutex_;
};

struct AsyncNotifier {
  enum class Status {
    Timeout,
    Normal,
  };
  virtual Status blockedWaitFor(const std::chrono::milliseconds &) {
    return AsyncNotifier::Status::Timeout;
  }
  virtual Status
  blockedWaitUtil(const std::chrono::system_clock::time_point &) {
    return AsyncNotifier::Status::Timeout;
  }
  virtual void wake() const = 0;
  virtual ~AsyncNotifier() = default;
};

struct Notifier final : AsyncNotifier, MutexLockWrap {
  Status blockedWaitFor(const std::chrono::milliseconds &timeout) override {
    return blockedWaitUtil(std::chrono::system_clock::now() + timeout);
  }

  Status blockedWaitUtil(
      const std::chrono::system_clock::time_point &time_point) override {
    // if flag from false to false, wait for notification.
    // if flag from true to false, do nothing.
    auto res = AsyncNotifier::Status::Normal;
    if (!wait_flag_.exchange(false, std::memory_order_acq_rel)) {
      {
        auto lock = genUniqueLock();
        if (!wait_flag_.load(std::memory_order_acquire)) {
          if (cv_.wait_until(lock, time_point) == std::cv_status::timeout)
            res = AsyncNotifier::Status::Timeout;
        }
      }
      wait_flag_.store(false, std::memory_order_release);
    }
    return res;
  }

  void wake() const override {
    // if flag from false -> true, then wake up.
    // if flag from true -> true, do nothing.
    if (!wait_flag_.exchange(true, std::memory_order_acq_rel)) {
      // wake up notifier
      auto _ = genLockGuard();
      cv_.notify_one();
    }
  }

  virtual ~Notifier() = default;

private:
  mutable std::condition_variable cv_;
  // multi notifiers single receiver model. use another flag to avoid waiting
  // endlessly.
  mutable std::atomic_bool wait_flag_{false};
};
