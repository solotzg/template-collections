#pragma once

#include "headers.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>

struct BasicConfig {
  static constexpr size_t CPU_CACHE_LINE_SIZE = 64;
};
template <typename Base, size_t alignment> struct AlignedStruct {
  template <typename... Args>
  AlignedStruct(Args &&...args) : base_{std::forward<Args>(args)...} {}

  Base &base() { return base_; }
  const Base &base() const { return base_; }
  Base *operator->() { return &base_; }
  const Base *operator->() const { return &base_; }
  Base &operator*() { return base_; }
  const Base &operator*() const { return base_; }

  // Wrapped with struct to guarantee that it is aligned to `alignment`
  // DO NOT need padding byte
  alignas(alignment) Base base_;
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

  Mutex &mutex() const { return *mutex_; }

private:
  mutable AlignedStruct<Mutex, BasicConfig::CPU_CACHE_LINE_SIZE> mutex_;
};

class alignas(BasicConfig::CPU_CACHE_LINE_SIZE) SharedMutexLockWrap {
public:
  using Mutex = std::shared_mutex;

protected:
  std::shared_lock<Mutex> GenSharedLock() const {
    return std::shared_lock(mutex());
  }

  std::unique_lock<Mutex> GenUniqueLock() const {
    return std::unique_lock(mutex());
  }

  Mutex &mutex() const { return *mutex_; }

private:
  mutable AlignedStruct<Mutex, BasicConfig::CPU_CACHE_LINE_SIZE> mutex_;
};

class MutexCondVarWrap : public MutexLockWrap {
public:
  using CondVar = std::condition_variable;

protected:
  CondVar &cv() const { return *cv_; }

private:
  mutable AlignedStruct<CondVar, BasicConfig::CPU_CACHE_LINE_SIZE> cv_;
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

struct Notifier final : AsyncNotifier, MutexCondVarWrap {
  Status BlockedWaitFor(const std::chrono::milliseconds &timeout) override {
    return BlockedWaitUtil(std::chrono::steady_clock::now() + timeout);
  }

  Status BlockedWaitUtil(
      const std::chrono::steady_clock::time_point &time_point) override {
    // if flag from false to false, wait for notification.
    // if flag from true to false, do nothing.
    auto res = AsyncNotifier::Status::Normal;
    if (!is_awake_->exchange(false, std::memory_order_acq_rel)) {
      {
        auto lock = GenUniqueLock();
        if (!is_awake_->load(std::memory_order_acquire)) {
          if (cv().wait_until(lock, time_point) == std::cv_status::timeout)
            res = AsyncNotifier::Status::Timeout;
        }
      }
      is_awake_->store(false, std::memory_order_release);
    }
    return res;
  }

  void Wake() const override {
    // if flag from false -> true, then wake up.
    // if flag from true -> true, do nothing.
    if (IsAwake()) {
      return;
    }
    if (!is_awake_->exchange(true, std::memory_order_acq_rel)) {
      // wake up notifier
      auto _ = GenLockGuard();
      cv().notify_one();
    }
  }

  bool IsAwake() const override {
    return is_awake_->load(std::memory_order_acquire);
  }

  virtual ~Notifier() = default;

private:
  // multi notifiers single receiver model. use another flag to avoid waiting
  // endlessly.
  mutable AlignedStruct<std::atomic_bool, BasicConfig::CPU_CACHE_LINE_SIZE>
      is_awake_{false};
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

struct Waiter : MutexCondVarWrap {
  void Wait() {
    auto lock = GenUniqueLock();
    cv().wait(lock, [&] { return start_; });
  }
  void WakeAll() {
    auto lock = GenLockGuard();
    start_ = true;
    cv().notify_all();
  }

protected:
  bool start_ = false;
};

struct TimeCost {
  using Clock = std::chrono::steady_clock;
  //
  TimeCost(const char *label = "ROOT");
  ~TimeCost();
  void Show(const char *prefix = nullptr) const;
  void Reset();
  Clock::duration Duration();

private:
  const char *label_;
  Clock::time_point start_;
};

namespace variant_op {
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
template <class T> struct always_false : std::false_type {};
} // namespace variant_op

void ShowBuildInfo(std::ostream &os);