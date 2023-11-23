#pragma once

#include "utils.h"

namespace utils {

template <typename T, typename Allocator = std::allocator<T>> struct SPSCQueue {
  using Element = T;

  struct SPSCQueueCustomer {
    bool IsEmpty() { return spsc_queue_.IsEmpty(); }
    std::optional<T> Get() { return spsc_queue_.Get(); }
    template <typename... Args> bool Get(Args &&...args) {
      return spsc_queue_.Get(std::forward<Args>(args)...);
    }

  private:
    friend struct SPSCQueue;
    SPSCQueueCustomer(SPSCQueue &spsc_queue) : spsc_queue_(spsc_queue) {}
    SPSCQueueCustomer(const SPSCQueueCustomer &) = delete;
    SPSCQueue &spsc_queue_;
  };

  struct SPSCQueueProducer {
    bool IsFull() { return spsc_queue_.IsFull(); }
    template <typename... Args> bool Put(Args &&...args) {
      return spsc_queue_.Put(std::forward<Args>(args)...);
    }

  private:
    friend struct SPSCQueue;
    SPSCQueueProducer(SPSCQueue &spsc_queue) : spsc_queue_(spsc_queue) {}
    SPSCQueueProducer(const SPSCQueueProducer &) = delete;
    SPSCQueue &spsc_queue_;
  };

  SPSCQueueProducer GenProducer() { return SPSCQueueProducer{*this}; }

  SPSCQueueCustomer GenCustomer() { return SPSCQueueCustomer{*this}; }

  static size_t CalcMaxNum(size_t cap) {
    assert(cap < std::numeric_limits<uint32_t>::max());
    if (cap < 2)
      return 2;
    return utils::NextPow2(cap);
  }

  explicit SPSCQueue(uint32_t cap, const Allocator &allocator = Allocator())
      : allocator_(allocator), max_num_(CalcMaxNum(cap)),
        buffer_mask_(max_num_ - 1) {
    buff_.Allocate(max_num_, allocator_);
  }

  SPSCQueue(const SPSCQueue &) = delete;

  ~SPSCQueue() {
    for (;;) {
      const auto index = head().load(std::memory_order_relaxed);
      if (index == tail().load(std::memory_order_relaxed))
        break;
      GetAtImpl(index, [&](T &) constexpr {});
      head().store((index + 1) & buffer_mask_, std::memory_order_release);
    }
    buff_.Deallocate(max_num_, allocator_);
  }

  void Format(fmt::memory_buffer &out) const {
    bool is_first = true;
    for (auto cur = head().load(std::memory_order_relaxed);
         cur != tail().load(std::memory_order_acquire);
         cur = (cur + 1) & buffer_mask_) {
      FMT_IF_APPEND(out, !is_first, ", ");
      FMT_APPEND(out, "`{}`", buff_[cur]);
      is_first = false;
    }
  }

  static const char *name() { return "SPSCQueue"; }

  size_t capacity() const { return max_num_ - 1; }

protected:
  bool LoadValidHead(size_t &cur) {
    cur = head().load(std::memory_order_relaxed);
    if (cur == tail_cache()) {
      tail_cache() = tail().load(std::memory_order_acquire);
      if (cur == tail_cache())
        return false;
    }
    return true;
  }

  bool LoadValidTail(size_t &cur) {
    cur = tail().load(std::memory_order_relaxed);
    auto next = (cur + 1) & buffer_mask_;
    if (next == head_cache()) {
      head_cache() = head().load(std::memory_order_acquire);
      if (next == head_cache()) {
        return false;
      }
    }
    return true;
  }

  std::optional<T> Get() {
    size_t pos{};
    if (!LoadValidHead(pos))
      return std::nullopt;
    std::optional<T> res{};
    GetAtImpl(pos, [&](T &v) { res.emplace(std::move(v)); });
    head().store((pos + 1) & buffer_mask_, std::memory_order_release);
    return res;
  }

  bool Get(T &res) {
    size_t pos{};
    if (!LoadValidHead(pos))
      return false;
    GetAtImpl(pos, [&](T &v) { res = std::move(v); });
    head().store((pos + 1) & buffer_mask_, std::memory_order_release);
    return true;
  }

  bool Put(T &&data) {
    size_t pos{};
    if (!LoadValidTail(pos))
      return false;
    buff_.New(pos, std::move(data));
    tail().store((pos + 1) & buffer_mask_, std::memory_order_release);
    return true;
  }

private:
  template <typename F> void GetAtImpl(size_t index, F &&f) {
    static_assert(std::is_destructible_v<T>);
    f(buff_[index]);
    buff_.Del(index);
  }

  std::atomic_size_t &head() { return *head_; }
  const std::atomic_size_t &head() const { return *head_; }
  std::atomic_size_t &tail() { return *tail_; }
  const std::atomic_size_t &tail() const { return *tail_; }
  size_t &head_cache() { return *head_cache_; }
  const size_t &head_cache() const { return *head_cache_; }
  size_t &tail_cache() { return *tail_cache_; }
  const size_t &tail_cache() const { return *tail_cache_; }

private:
  Allocator allocator_;
  const size_t max_num_;
  const size_t buffer_mask_;
  ConstSizeArray<T, Allocator> buff_;
  /// Aligned to CPU cache line size
  AlignedStruct<std::atomic_size_t, BasicConfig::CPU_CACHE_LINE_SIZE> head_{};
  AlignedStruct<std::atomic_size_t, BasicConfig::CPU_CACHE_LINE_SIZE> tail_{};
  AlignedStruct<size_t, BasicConfig::CPU_CACHE_LINE_SIZE> head_cache_{};
  AlignedStruct<size_t, BasicConfig::CPU_CACHE_LINE_SIZE> tail_cache_{};
};

template <typename SPSC> struct SPSCQueueWithNotifer {
  using Element = typename SPSC::Element;

  template <typename CBFull>
  bool Put(Element &&e, const Milliseconds &timeout, CBFull &&cb_when_full) {
    if (auto res = TryPut(std::forward<Element>(e)); res) {
      return res;
    }
    auto time_point = SteadyClock::now() + timeout;
    return Put(std::forward<Element>(e), time_point, std::move(cb_when_full));
  }

  template <typename CBFull>
  bool Put(Element &&e, const SteadyClock::time_point &time_point,
           CBFull &&cb_when_full) {
    for (;;) {
      if (auto res = TryPut(std::forward<Element>(e)); res) {
        return res;
      } else {
        cb_when_full();
      }
      switch (producer_notifier_.BlockedWaitUtil(time_point)) {
      case Notifier::Status::Timeout: {
        return false;
      }
      case Notifier::Status::Normal: {
        break;
      }
      }
    }
  }
  bool TryPut(Element &&e) {
    return spsc_queue_.GenProducer().Put(std::forward<Element>(e));
  }

  void WakeProducer() const { producer_notifier_.Wake(); }

  template <typename NotifierType, typename CallBackWhenEmpty>
  std::optional<Element> Get(const Milliseconds &timeout,
                             NotifierType &&customer_notifier,
                             CallBackWhenEmpty &&cb_empty) {
    if (auto res = TryGet(); res) {
      return res;
    }
    auto time_point = SteadyClock::now() + timeout;
    return Get(time_point, std::move(customer_notifier), std::move(cb_empty));
  }

  template <typename NotifierType, typename CallBackWhenEmpty>
  std::optional<Element> Get(const SteadyClock::time_point &time_point,
                             NotifierType &&customer_notifier,
                             CallBackWhenEmpty &&cb_empty) {
    for (;;) {
      if (auto res = TryGet(); res) {
        return res;
      } else {
        cb_empty();
      }
      switch (customer_notifier.BlockedWaitUtil(time_point)) {
      case Notifier::Status::Timeout: {
        return std::nullopt;
      }
      case Notifier::Status::Normal: {
        break;
      }
      }
    }
  }

  std::optional<Element> TryGet() { return spsc_queue_.GenCustomer().Get(); }

  template <typename... Args>
  SPSCQueueWithNotifer(Args &&...args)
      : spsc_queue_(std::forward<Args>(args)...) {}

  SPSC spsc_queue_;
  Notifier producer_notifier_;
};

} // namespace utils
