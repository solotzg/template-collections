#pragma once

#include <utils/build_info.h>
#include <utils/head_template.h>
#include <utils/scope_guard.h>
#include <utils/thread-name.h>

namespace utils {

static constexpr Milliseconds MaxDuration = Seconds{72 * 3600}; // 72 hours

constexpr char CRLF = '\n';

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
  AlignedStruct(Args &&...args) : base_(std::forward<Args>(args)...) {}

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
  static constexpr const bool kCanWaitForDuration = false;
  // thread safe
  virtual bool Wake() const = 0;
};

struct Notifier final : AsyncNotifier, MutexCondVarWrap {
  static constexpr const bool kCanWaitForDuration = true;

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
    if (!WaitAndExchangeAwake(false)) {
      {
        auto lock = GenUniqueLock();
        auto s = cv().wait_until(lock, time_point, [&] {
          return is_awake_->load(std::memory_order_acquire);
        });
        if (!s) {
          res = Status::Timeout;
        }
      }
      WaitAndExchangeAwake(false);
    }
    return res;
  }

  void BlockedWait() {
    if (!WaitAndExchangeAwake(false)) {
      {
        auto lock = GenUniqueLock();
        cv().wait(lock,
                  [&] { return is_awake_->load(std::memory_order_acquire); });
      }
      WaitAndExchangeAwake(false);
    }
  }

  bool TestAndWake() const {
    if (IsAwake()) {
      return false;
    }
    is_awake_->store(true, std::memory_order_release);
    RunWithMutexLock([&] { cv().notify_one(); });
    return true;
  }

  bool Wake() const override {
    // if flag from false -> true, then wake up.
    // if flag from true -> true, do nothing.
    if (!WaitAndExchangeAwake(true)) {
      // wake up notifier
      RunWithMutexLock([&] { cv().notify_one(); });
      return true;
    }
    return false;
  }

  bool IsAwake() const {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return is_awake_->load(std::memory_order_seq_cst);
  }

  static NotifierPtr New() { return std::make_shared<Notifier>(); }

  virtual ~Notifier() = default;

  bool WaitAndExchangeAwake(bool f) const {
    return is_awake_->exchange(f, std::memory_order_acq_rel);
  }

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

    /*
      Use the `xchg` instruction to wait for all modifications before setting
      awake flag to TRUE.
     */
    if (!WaitAndExchangeAwake(false)) {
      is_awake_->wait(false);
      WaitAndExchangeAwake(false);
    }
  }

  bool Wake() const override {
    // if flag from false -> true, then wake up.
    // if flag from true -> true, do nothing.
    if (!WaitAndExchangeAwake(true)) {
      // wake up notifier
      is_awake_->notify_one();
      return true;
    }
    return false;
  }

  bool TestAndWake() const {
    if (IsAwake()) {
      return false;
    }
    is_awake_->store(true, std::memory_order_release);
    is_awake_->notify_one();
    return true;
  }

  bool IsAwake() const {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return is_awake_->load(std::memory_order_seq_cst);
  }

  static AtomicNotifierPtr New() { return std::make_shared<Self>(); }

  bool WaitAndExchangeAwake(bool f) const {
    /*
         0:	31 c0                	xor    %eax,%eax
         2:	86 07                	xchg   %al,(%rdi)
         4:	c3                   	retq

      If a memory operand is referenced, the processor’s locking protocol is
      automatically implemented for the duration of the exchange operation,
      regardless of the presence or absence of the LOCK prefix or of the value
      of the IOPL.

      According to `MESI` protocol, the behavior about `LOCK` will apply all
      modifications before reading. It means that what happened before `xchg`
      must be observable.
    */
    return is_awake_->exchange(f, std::memory_order_acq_rel);
  }

private:
  mutable CPUCacheLineAligned<std::atomic_bool> is_awake_{false};
};

struct AtomicOnceNotify {
  enum class Status {
    Idle,
    Waiting,
    Ready,
  };

  void WaitForReady() {
    if (status_->load(std::memory_order_seq_cst) == Status::Ready)
      return;
    if (WaitAndExchangeAwake(Status::Waiting) != Status::Ready) {
      status_->wait(Status::Waiting);
    }
  }

  void SetReady() const {
    const auto ori_status = WaitAndExchangeAwake(Status::Ready);
    if (ori_status == Status::Waiting) {
      status_->notify_one();
    }
  }

  void Reset() { status_->store(Status::Idle, std::memory_order_release); }

private:
  Status WaitAndExchangeAwake(Status s) const {
    return status_->exchange(s, std::memory_order_seq_cst);
  }

private:
  mutable CPUCacheLineAligned<std::atomic<Status>> status_{Status::Idle};
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

template <typename T> static T NextPow2(T v) { return std::bit_ceil(v); }

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
  using Seconds = double;
  //
  TimeCost(std::string_view label = "ROOT", bool auto_show = true);
  ~TimeCost();
  void Show(const char *prefix = nullptr) const;
  void Show(TimeCost::Clock::duration dur, const char *prefix = nullptr) const;
  void Reset();
  /// nanoseconds
  Clock::duration Duration();
  /// seconds float64
  Seconds DurationSec();
  static Seconds IntoDurationSec(const TimeCost::Clock::duration);

private:
  std::string_view label() const;

private:
  std::unique_ptr<std::string> label_;
  Clock::time_point start_;
  const bool auto_show_;
};

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
void ToLower(std::string &s);
std::string ToLower(std::string_view s);

void SerializeTimepoint(char *data, utils::SysMilliseconds &last_update_time,
                        const utils::SysMilliseconds time_point_ms);

inline void SerializeTimepoint(char *data,
                               utils::SysMilliseconds &last_update_time,
                               const SystemClock::time_point time_point) {
  SerializeTimepoint(
      data, last_update_time,
      std::chrono::time_point_cast<utils::Milliseconds>(time_point));
}

extern MutexLockWrap global_stdout_lock;

template <typename F> static auto RunWithGlobalStdoutLock(F &&f) {
  return global_stdout_lock.RunWithMutexLock(std::forward<F>(f));
}

template <size_t buffer_min_size = 200> struct STDCoutGuard : noncopyable {
  using memory_buffer = fmt::basic_memory_buffer<char, buffer_min_size>;

  const memory_buffer &buffer() const { return buffer_; }
  memory_buffer &buffer() { return buffer_; }
  utils::SysMilliseconds &last_update_time_sec() { return last_update_time_; }
  const utils::SysMilliseconds &last_update_time_sec() const {
    return last_update_time_;
  }

  static STDCoutGuard &thread_local_instance() {
    thread_local STDCoutGuard obj;
    return obj;
  }

private:
  memory_buffer buffer_;
  utils::SysMilliseconds last_update_time_;
};

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

  template <typename... Args> T &New(size_t index, Args &&...args) {
    ASSERT(data_);
    new (data_ + index) T(std::forward<Args>(args)...);
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

template <size_t SIZEOF_OBJ, size_t ALIGNOF_OBJ,
          size_t MAX_PAGE_SIZE = 64 * 1024>
struct FastBinBase : noncopyable {
  static constexpr size_t ADDR_SIZE = sizeof(void *);
  static constexpr size_t ADDR_ALIGN = alignof(void *);

  static constexpr size_t OBJ_SIZE =
      (SIZEOF_OBJ + ADDR_ALIGN - 1) / ADDR_ALIGN * ADDR_ALIGN;
  static constexpr size_t OBJ_ALIGN =
      (ALIGNOF_OBJ + ADDR_ALIGN - 1) / ADDR_ALIGN * ADDR_ALIGN;

  FastBinBase() {
    const size_t require = OBJ_SIZE * 32 + ADDR_SIZE + OBJ_ALIGN;
    for (next_alloc_page_size_ = 32; next_alloc_page_size_ < require;) {
      next_alloc_page_size_ *= 2;
    }
  }

  ~FastBinBase() {
    while (pages_) {
      void *page = pages_;
      void *next = next_addr_of(page);
      pages_ = next;
      free(page);
    }
  }

  void *Alloc() {
    void *obj = next_;
    if (obj) {
      next_ = next_addr_of(obj);
      return obj;
    }
    if (start_ + OBJ_SIZE > endup_) {
      auto *page = malloc(next_alloc_page_size_);
      next_addr_of(page) = pages_;
      pages_ = page;

      size_t first_obj_ptr =
          ((size_t)(page) + ADDR_SIZE + OBJ_ALIGN - 1) / OBJ_ALIGN * OBJ_ALIGN;
      start_ = (char *)(first_obj_ptr);
      endup_ = (char *)(page) + next_alloc_page_size_;
      if (next_alloc_page_size_ < MAX_PAGE_SIZE) {
        next_alloc_page_size_ *= 2;
      }
    }
    obj = start_;
    start_ += OBJ_SIZE;

    return obj;
  }

  void Dealloc(void *ptr) {
    next_addr_of(ptr) = next_;
    next_ = ptr;
  }

protected:
  static void *&next_addr_of(void *ptr) { return (((void **)(ptr))[0]); }

protected:
  size_t next_alloc_page_size_;
  char *start_{};
  char *endup_{};
  void *next_{};
  void *pages_{};
};

template <typename T> struct FastBin : FastBinBase<sizeof(T), alignof(T)> {
  using ObjectType = T;
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

  bool IsLocked() const { return lock().load(std::memory_order_seq_cst); }

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
    return !lock().exchange(true, std::memory_order_acq_rel);
  }

  void Lock() const {
    for (; lock().exchange(true, std::memory_order_acq_rel);) {
      std::this_thread::yield();
    }
  }

  void UnLock() const {
    ASSERT(IsLocked());
    lock().store(false, std::memory_order_release);
  }

  std::atomic_bool &lock() const { return *lock_; }

protected:
  mutable CPUCacheLineAligned<std::atomic_bool> lock_;
};

template <bool enable = true> struct ConsistencyChecker {
  ConsistencyChecker() { inc(); }
  ~ConsistencyChecker() { dec(); }
  static I64 obj_cnt() { return global_obj_cnt(); }

private:
  struct Holder : utils::noncopyable {
    ~Holder() {
      RUNTIME_ASSERT_EQ(obj_cnt_.load(std::memory_order_seq_cst), 0);
    }
    Holder() = default;
    std::atomic_int64_t obj_cnt_{};
  };

private:
  void inc() { ++global_obj_cnt(); }
  void dec() { --global_obj_cnt(); }
  static std::atomic_int64_t &global_obj_cnt() {
    static Holder s_holder;
    return s_holder.obj_cnt_;
  };
};

template <> struct ConsistencyChecker<false> {};

template <typename Allocator> struct SharedObjectPtr;

template <typename T> struct SharedObject : utils::noncopyable {
  using TargetType = T;

  template <typename... Args>
  SharedObject(void *allocator, Args &&...args)
      : allocator_(allocator), obj_(std::forward<Args>(args)...) {
    ASSERT(allocator_);
  }
  TargetType *operator->() { return &obj_; }
  const TargetType *operator->() const { return &obj_; }

  std::atomic_int64_t &ref_cnt() { return ref_cnt_; }
  const std::atomic_int64_t &ref_cnt() const { return ref_cnt_; }
  void *allocator() const { return allocator_; };

private:
  void *allocator_{};
  std::atomic_int64_t ref_cnt_{1};
  T obj_;
};

template <typename Allocator> struct SharedObjectPtr {
  using Object = typename Allocator::ObjectType;
  using TargetType = typename Object::TargetType;

  template <typename... Args>
  static SharedObjectPtr New(Allocator &allocator, Args &&...args) {
    auto *p = reinterpret_cast<Object *>(allocator.Alloc());
    new (p) Object(&allocator, std::forward<Args>(args)...);
    ASSERT(p->ref_cnt() == 1);
    return SharedObjectPtr(p);
  }

  SharedObjectPtr(const SharedObjectPtr &src) : ptr_(src.ptr_) {
    if (ptr_)
      ptr_->ref_cnt()++;
  }
  SharedObjectPtr(SharedObjectPtr &&src)
      : ptr_(std::exchange(src.ptr_, nullptr)) {}

  SharedObjectPtr &operator=(const SharedObjectPtr &src) {
    Reset();
    ptr_ = src.ptr_;

    if (ptr_)
      ptr_->ref_cnt()++;
    return *this;
  }
  SharedObjectPtr &operator=(SharedObjectPtr &&src) {
    Reset();
    ptr_ = std::exchange(src.ptr_, nullptr);
    return *this;
  }

  SharedObjectPtr(std::nullptr_t) : ptr_(nullptr) {}

  TargetType *operator->() { return (*ptr_).operator->(); }
  const TargetType *operator->() const { return (*ptr_).operator->(); }

  ~SharedObjectPtr() { Reset(); }
  SharedObjectPtr() = default;

private:
  void Reset() {
    if (!ptr_)
      return;
    if (auto n = --ptr_->ref_cnt(); !n) {
      auto *allocator_ = (Allocator *)(ptr_->allocator());
      ptr_->~Object();
      allocator_->Dealloc(ptr_);
      ptr_ = nullptr;
    }
  }

  SharedObjectPtr(Object *ptr) : ptr_(ptr) {}

private:
  Object *ptr_{};
};

ALWAYS_INLINE
uint64_t hardware_timestamp_counter() {
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
  return __builtin_ia32_rdtsc();
#elif defined(__aarch64__)
  uint64_t cval;
  asm volatile("mrs %0, cntvct_el0" : "=r"(cval));
  return cval;
#else
  static_assert(false);
#endif
}

} // namespace utils