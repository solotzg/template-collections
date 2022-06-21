#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>

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
  virtual Status BlockedWaitFor(const std::chrono::milliseconds &) {
    return AsyncNotifier::Status::Timeout;
  }
  virtual Status
  BlockedWaitUtil(const std::chrono::system_clock::time_point &) {
    return AsyncNotifier::Status::Timeout;
  }
  virtual void Wake() const = 0;
  virtual ~AsyncNotifier() = default;
};

struct Notifier final : AsyncNotifier, MutexLockWrap {
  Status BlockedWaitFor(const std::chrono::milliseconds &timeout) override {
    return BlockedWaitUtil(std::chrono::system_clock::now() + timeout);
  }

  Status BlockedWaitUtil(
      const std::chrono::system_clock::time_point &time_point) override {
    // if flag from false to false, wait for notification.
    // if flag from true to false, do nothing.
    auto res = AsyncNotifier::Status::Normal;
    if (!wait_flag_.exchange(false, std::memory_order_acq_rel)) {
      {
        auto lock = GenUniqueLock();
        if (!wait_flag_.load(std::memory_order_acquire)) {
          if (cv_.wait_until(lock, time_point) == std::cv_status::timeout)
            res = AsyncNotifier::Status::Timeout;
        }
      }
      wait_flag_.store(false, std::memory_order_release);
    }
    return res;
  }

  void Wake() const override {
    // if flag from false -> true, then wake up.
    // if flag from true -> true, do nothing.
    if (!wait_flag_.exchange(true, std::memory_order_acq_rel)) {
      // wake up notifier
      auto _ = GenLockGuard();
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
