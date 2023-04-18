#pragma once

#include "build_info.h"
#include "head_template.h"
#include "scope_guard.h"
#include "thread-name.h"

namespace utils {

static constexpr Milliseconds MaxDuration = Seconds{72 * 3600}; // 72 hours

struct AtomicNotifier;
using AtomicNotifierPtr = std::shared_ptr<AtomicNotifier>;

struct AsyncNotifier;
using AsyncNotifierPtr = std::shared_ptr<AsyncNotifier>;

struct Notifier;
using NotifierPtr = std::shared_ptr<Notifier>;

struct Waiter;
using WaiterPtr = std::shared_ptr<Waiter>;

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

private:
  // Wrapped with struct to guarantee that it is aligned to `alignment`
  // DO NOT need padding byte
  alignas(alignment) Base base_;
};

template <typename Base>
using CPUCacheLineAligned =
    AlignedStruct<Base, BasicConfig::CPU_CACHE_LINE_SIZE>;

class MutexLockWrap {
public:
  using Mutex = std::mutex;

  template <typename F> auto RunWithMutexLock(F &&f) const {
    auto lock = GenLockGuard();
    return f();
  }

  template <typename F> auto RunWithUniqueLock(F &&f) const {
    auto lock = GenUniqueLock();
    return f(lock);
  }

  template <typename F> auto RunWithTryLock(F &&f) const {
    auto lock = TryToLock();
    return f(lock);
  }

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
  mutable CPUCacheLineAligned<Mutex> mutex_;
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

  Mutex &mutex() const { return *mutex_; }

private:
  mutable CPUCacheLineAligned<Mutex> mutex_;
};

class MutexCondVarWrap : public MutexLockWrap {
public:
  using CondVar = std::condition_variable;

protected:
  CondVar &cv() const { return *cv_; }

private:
  mutable CPUCacheLineAligned<CondVar> cv_;
};

struct AsyncNotifier {
  // thread safe
  virtual bool IsAwake() const = 0;
  virtual bool Wake() const = 0;
};

struct Notifier final : AsyncNotifier, MutexCondVarWrap {
  enum class Status {
    Timeout,
    Normal,
  };

  Status BlockedWaitFor(const Milliseconds &timeout) {
    return BlockedWaitUtil(SteadyClock::now() + timeout);
  }

  Status BlockedWaitUtil(const SteadyClock::time_point &time_point) {
    // if flag from false to false, wait for notification.
    // if flag from true to false, do nothing.
    auto res = Status::Normal;
    if (!is_awake_->exchange(false, std::memory_order_acq_rel)) {
      RunWithUniqueLock([&](auto &lock) {
        if (!is_awake_->load(std::memory_order_acquire)) {
          if (cv().wait_until(lock, time_point) == std::cv_status::timeout)
            res = Status::Timeout;
        }
      });
      is_awake_->store(false, std::memory_order_release);
    }
    return res;
  }

  void BlockedWait() {
    if (!is_awake_->exchange(false, std::memory_order_acq_rel)) {
      RunWithUniqueLock([&](auto &lock) {
        if (!is_awake_->load(std::memory_order_acquire)) {
          cv().wait(lock);
        }
      });
      is_awake_->store(false, std::memory_order_release);
    }
  }

  bool Wake() const override {
    // if flag from false -> true, then wake up.
    // if flag from true -> true, do nothing.
    if (IsAwake()) {
      return false;
    }
    if (!is_awake_->exchange(true, std::memory_order_acq_rel)) {
      // wake up notifier
      RunWithMutexLock([&] { cv().notify_one(); });
      return true;
    }
    return false;
  }

  bool IsAwake() const override {
    return is_awake_->load(std::memory_order_acquire);
  }

  static NotifierPtr New() { return std::make_shared<Notifier>(); }

  virtual ~Notifier() = default;

private:
  // multi notifiers single receiver model. use another flag to avoid waiting
  // endlessly.
  mutable CPUCacheLineAligned<std::atomic_bool> is_awake_{false};
};

static inline std::ostream &operator<<(std::ostream &ostr,
                                       Notifier::Status status) {
  static const char *AsyncNotifierStatusNames[] = {"Timeout", "Normal"};
  ostr << AsyncNotifierStatusNames[static_cast<int>(status)];
  return ostr;
}

struct AtomicNotifier final : AsyncNotifier {
  using Self = AtomicNotifier;

  void BlockedWait() {
    // if flag from false to false, wait for notification.
    // if flag from true to false, do nothing.
    if (!is_awake_->exchange(false, std::memory_order_acq_rel)) {
      is_awake_->wait(false);
      is_awake_->store(false, std::memory_order_release);
    }
  }

  bool Wake() const override {
    // if flag from false -> true, then wake up.
    // if flag from true -> true, do nothing.
    if (IsAwake()) {
      return false;
    }
    if (!is_awake_->exchange(true, std::memory_order_acq_rel)) {
      // wake up notifier
      is_awake_->notify_one();
      return true;
    }
    return false;
  }

  bool IsAwake() const override {
    return is_awake_->load(std::memory_order_acquire);
  }

  static AtomicNotifierPtr New() { return std::make_shared<Self>(); }

private:
  mutable CPUCacheLineAligned<std::atomic_bool> is_awake_{false};
};

struct NotifierWap {
protected:
  const Notifier &notifier() const { return notifier_; }
  Notifier &notifier() { return notifier_; }

private:
  Notifier notifier_;
};

struct AtomicNotifierWap {
protected:
  const AtomicNotifier &notifier() const { return notifier_; }
  AtomicNotifier &notifier() { return notifier_; }

private:
  AtomicNotifier notifier_;
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

struct Waiter {
  void Wait() {
    if (start_)
      return;
    start_.wait(false);
  }
  void WakeAll() {
    start_ = true;
    start_.notify_all();
  }
  void WakeOne() {
    start_ = true;
    start_.notify_one();
  }
  static WaiterPtr New() { return std::make_shared<Waiter>(); }

protected:
  std::atomic_bool start_{};
};

struct TimeCost {
  using Clock = SteadyClock;
  //
  TimeCost(std::string_view label = "ROOT", bool auto_show = true);
  ~TimeCost();
  void Show(const char *prefix = nullptr) const;
  void Reset();
  Clock::duration Duration();

private:
  std::string_view label_;
  Clock::time_point start_;
  const bool auto_show_;
};

namespace variant_op {
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
template <class T> struct always_false : std::false_type {};
} // namespace variant_op

void ShowBuildInfo(std::ostream &os);

class noncopyable {
protected:
  constexpr noncopyable() = default;
  ~noncopyable() = default;
  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;
};

struct AsyncThreadRunner {
  enum class Status {
    Idle,
    Running,
    Stop,
  };

  bool IsStop() const { return status() == Status::Stop; }
  Status status() const { return status_; }

protected:
  template <typename F>
  bool AsyncRunImpl(F &&f, std::string_view thread_name = {}) {
    return mutex_.RunWithMutexLock([this, f, thread_name] {
      if (auto ori_status = status_.exchange(Status::Running);
          ori_status == Status::Running)
        return false;
      ASSERT(!context_);
      context_.emplace(std::move(f));
      if (!thread_name.empty())
        setThreadName(context_->get_id(), thread_name);
      return true;
    });
  }

  template <typename F1, typename F2> bool StopAndWaitImpl(F1 &&f1, F2 &&f2) {
    return mutex_.RunWithMutexLock([&] {
      if (auto ori_status = status_.exchange(Status::Stop);
          ori_status == Status::Stop)
        return false;
      f1();
      if (context_) {
        context_->join();
        context_.reset();
      }
      f2();
      return true;
    });
  }

protected:
  std::optional<std::thread> context_;
  std::atomic<Status> status_{};
  MutexLockWrap mutex_;
};

template <typename F> NO_INLINE static auto NO_INLINE_FUNC(F &&f) {
  return f();
}

void ToUpper(std::string &s);
std::string ToUpper(std::string_view s);

inline void STDCout(std::string_view s) {
  static MutexLockWrap lock;
  lock.RunWithMutexLock([&] { std::cout << s << std::endl; });
}

template <typename T, typename Allocator = std::allocator<T>>
struct ConstSizeArray {
  using Element = T;

  T &operator[](size_t index) { return data_[index]; }

  const T &operator[](size_t index) const { return data_[index]; }

  void Allocate(size_t n, Allocator &&allocator = Allocator{}) {
    Allocate(n, allocator);
  }

  void Allocate(size_t n, Allocator &allocator) {
    data_ = allocator.allocate(n);
  }

  template <typename... Args> T &New(size_t index, Args &&...__args) {
    ASSERT(data_);
    new (data_ + index) T(std::forward<Args>(__args)...);
    return data_[index];
  }

  void Del(size_t index) {
    ASSERT(data_);
    data_[index].~T();
  }

  void Deallocate(size_t n, Allocator &&allocator = Allocator{}) {
    return Deallocate(n, allocator);
  }

  void Deallocate(size_t n, Allocator &allocator) {
    if (!data_)
      return;
    allocator.deallocate(data_, n);
    data_ = nullptr;
  }

  bool operator()() { return data_; }
  bool operator!() { return !data_; }

private:
  T *data_{};
};

template <typename T> struct FastBin : noncopyable {
  static constexpr size_t MAX_PAGE_SIZE = 64 * 1024;
  static constexpr size_t ADDR_SIZE = sizeof(void *);
  static constexpr size_t ADDR_ALIGN = alignof(void *);

  static constexpr size_t OBJ_SIZE =
      (sizeof(T) + ADDR_ALIGN - 1) / ADDR_ALIGN * ADDR_ALIGN;
  static constexpr size_t OBJ_ALIGN =
      (alignof(T) + ADDR_ALIGN - 1) / ADDR_ALIGN * ADDR_ALIGN;

  FastBin() {
    const size_t require = OBJ_SIZE * 32 + ADDR_SIZE + OBJ_ALIGN;
    for (page_size_ = 32; page_size_ < require;) {
      page_size_ *= 2;
    }
  }

  ~FastBin() {
    while (pages_) {
      void *page = pages_;
      void *next = calc_next(page);
      pages_ = next;
      free(page);
    }
  }

  void *Alloc() {
    void *obj = next_;
    if (obj) {
      next_ = calc_next(obj);
      return obj;
    }
    if (start_ + OBJ_SIZE > endup_) {
      char *page = (char *)(malloc(page_size_));
      calc_next(page) = pages_;
      pages_ = page;

      size_t lineptr =
          ((size_t)(page) + ADDR_SIZE + OBJ_ALIGN - 1) / OBJ_ALIGN * OBJ_ALIGN;
      start_ = (char *)(lineptr);
      endup_ = page + page_size_;
      if (page_size_ < MAX_PAGE_SIZE) {
        page_size_ *= 2;
      }
    }
    obj = start_;
    start_ += OBJ_SIZE;

    return obj;
  }

  void Dealloc(void *ptr) {
    calc_next(ptr) = next_;
    next_ = ptr;
  }

protected:
  static void *&calc_next(void *ptr) { return (((void **)(ptr))[0]); }

protected:
  size_t page_size_;
  char *start_{};
  char *endup_{};
  void *next_{};
  void *pages_{};
};

struct SpinLockWrap {
  template <bool = false> struct LockGuard {
    LockGuard(const SpinLockWrap &spin_lock) : spin_lock_(spin_lock) {
      spin_lock_.Lock();
    }
    ~LockGuard() { spin_lock_.UnLock(); }

  private:
    const SpinLockWrap &spin_lock_;
  };

  template <> struct LockGuard<true> {
    LockGuard(const SpinLockWrap &spin_lock)
        : spin_lock_(spin_lock), locked_(spin_lock_.TryLock()) {}
    ~LockGuard() {
      if (locked_)
        spin_lock_.UnLock();
    }
    bool TryLock() const {
      if (locked())
        return false;
      locked_ = spin_lock_.TryLock();
      return locked_;
    }
    bool locked() const { return locked_; }

  private:
    const SpinLockWrap &spin_lock_;
    mutable bool locked_;
  };

  bool IsLocked() const { return lock().load(std::memory_order::acquire); }

  template <typename F> auto RunWithLock(F &&f) const {
    auto lock = GenLockGuard();
    return f();
  }
  template <typename F> auto RunWithTryLock(F &&f) const {
    auto lock = GenTryLockGuard();
    return f(lock);
  }
  LockGuard<true> GenTryLockGuard() const { return LockGuard<true>(*this); }
  LockGuard<false> GenLockGuard() const { return LockGuard<false>(*this); }

protected:
  bool TryLock() const {
    if (IsLocked())
      return false;
    return !lock().exchange(true, std::memory_order::acq_rel);
  }

  void Lock() const {
    for (; lock().exchange(true, std::memory_order::acq_rel);) {
      std::this_thread::yield();
    }
  }

  void UnLock() const {
    ASSERT(IsLocked());
    lock().store(false, std::memory_order::release);
  }

  std::atomic_bool &lock() const { return *lock_; }

protected:
  mutable CPUCacheLineAligned<std::atomic_bool> lock_;
};

} // namespace utils