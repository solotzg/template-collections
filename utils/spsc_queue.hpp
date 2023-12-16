#pragma once

#include <utils/utils.h>

namespace utils {

/**
  head  ->  ... ->  tail  -> ... ->  head
  read()            write()

  head == tail   -> empty
  head == 1+tail -> full
*/
template <typename T, typename Allocator = std::allocator<T>> struct SPSCQueue {
  using Element = T;

  static constexpr size_t npos = -1;

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
    for (size_t i = read_index(); i != write_index(); i = next_index(i)) {
      GetAtImpl(i, [](T &&) constexpr {});
    }
    buff_.Deallocate(max_num_, allocator_);
  }

  void Format(fmt::memory_buffer &out) const {
    bool is_first = true;
    for (size_t i = read_index(); i != write_index(); i = next_index(i)) {
      FMT_IF_APPEND(out, !is_first, ", ");
      FMT_APPEND(out, "`{}`", buff_[i]);
      is_first = false;
    }
  }

  static const char *name() { return "SPSCQueue"; }

  size_t capacity() const { return max_num_ - 1; }

  size_t latest_read_index() const {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return read_index().load(std::memory_order_seq_cst);
  }

  size_t latest_write_index() const {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return write_index().load(std::memory_order_seq_cst);
  }

  // consumer
  template <typename F> bool Get(F &&cb) {
    const size_t p = ConsumerCheckEmpty();
    if (p == npos) {
      return false;
    }
    GetAtImpl(p, std::forward<F>(cb));
    PublishReadIndex(next_index(p));
    return true;
  }

  // consumer
  std::optional<T> Get() {
    std::optional<T> res;
    Get([&](T &&v) { res.emplace(std::move(v)); });
    return res;
  }

  // producer
  template <typename... Args> bool Put(Args &&...args) {
    const size_t p = ProducerCheckFull();
    if (p == npos) {
      return false;
    }
    buff_.New(p, std::forward<Args>(args)...);
    PublishWriteIndex(next_index(p));
    return true;
  }

protected:
  // consumer
  size_t ConsumerCheckEmpty() {
    const auto index = read_index().load(std::memory_order_relaxed);
    if (index == end_cache_for_read()) {
      end_cache_for_read() = write_index().load(std::memory_order_acquire);
      if (index == end_cache_for_read())
        return npos;
    }
    return index;
  }

  // for producer
  size_t ProducerCheckFull() {
    const auto index = write_index().load(std::memory_order_relaxed);
    const auto next = next_index(index);
    if (next == end_cache_of_write()) {
      end_cache_of_write() = read_index().load(std::memory_order_acquire);
      if (next == end_cache_of_write()) {
        return npos;
      }
    }
    return index;
  }

private:
  template <typename F> void GetAtImpl(size_t index, F &&f) {
    static_assert(std::is_destructible_v<T>);
    f(std::move(buff_[index]));
    buff_.Del(index);
  }

  struct CachedIndex {
    std::atomic_size_t index;
    size_t end_cache;
  };

  std::atomic_size_t &read_index() { return read_index_->index; }
  const std::atomic_size_t &read_index() const { return read_index_->index; }
  std::atomic_size_t &write_index() { return write_index_->index; }
  const std::atomic_size_t &write_index() const { return write_index_->index; }
  size_t &end_cache_of_write() { return write_index_->end_cache; }
  size_t &end_cache_for_read() { return read_index_->end_cache; }

  void PublishWriteIndex(size_t i) {
    write_index().store(i, std::memory_order_release);
  }

  void PublishReadIndex(size_t i) {
    read_index().store(i, std::memory_order_release);
  }

  size_t next_index(size_t i) const { return (i + 1) & buffer_mask_; }

private:
  [[no_unique_address]] Allocator allocator_;
  const size_t max_num_;
  const size_t buffer_mask_;
  ConstSizeArray<T, Allocator> buff_;
  // Aligned to CPU cache line size
  // consumer
  CPUCacheLineAligned<CachedIndex> read_index_;
  // producer
  CPUCacheLineAligned<CachedIndex> write_index_;
};

template <typename SPSC, typename ProducerNotifier = utils::Notifier>
struct SPSCQueueWithNotifer {
  using Element = typename SPSC::Element;

  // for producer
  template <typename CB> void Put(Element &&e, CB &&cb_when_full) {
    for (;;) {
      if (TryPut(std::forward<Element>(e))) {
        return;
      } else {
        cb_when_full();
      }
      producer_notifier_.BlockedWait();
    }
  }

  // for producer
  template <typename CBFull>
  bool Put(Element &&e, const Milliseconds &timeout, CBFull &&cb_when_full)
    requires ProducerNotifier::kCanWaitForDuration
  {
    if (TryPut(std::forward<Element>(e))) {
      return true;
    }
    auto time_point = SteadyClock::now() + timeout;
    return Put(std::forward<Element>(e), time_point, std::move(cb_when_full));
  }

  // for producer
  template <typename CBFull>
  bool Put(Element &&e, const SteadyClock::time_point &time_point,
           CBFull &&cb_when_full)
    requires ProducerNotifier::kCanWaitForDuration
  {
    for (;;) {
      if (TryPut(std::forward<Element>(e))) {
        return true;
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

  // for producer
  bool TryPut(Element &&e) { return spsc_queue_.Put(std::forward<Element>(e)); }

  // for consumer
  template <typename NotifierType, typename CallBackWhenEmpty>
  std::optional<Element> Get(const Milliseconds &timeout,
                             NotifierType &&consumer_notifier,
                             CallBackWhenEmpty &&cb_empty) {
    if (auto res = TryGet(); res) {
      return res;
    }
    auto time_point = SteadyClock::now() + timeout;
    return Get(time_point, std::forward<NotifierType>(consumer_notifier),
               std::forward<CallBackWhenEmpty>(cb_empty));
  }

  // for consumer
  template <typename NotifierType, typename CallBackWhenEmpty>
  std::optional<Element> Get(const SteadyClock::time_point &time_point,
                             NotifierType &&consumer_notifier,
                             CallBackWhenEmpty &&cb_empty) {
    for (;;) {
      if (auto res = TryGet(); res) {
        return res;
      } else {
        cb_empty();
      }
      switch (consumer_notifier.BlockedWaitUtil(time_point)) {
      case Notifier::Status::Timeout: {
        return std::nullopt;
      }
      case Notifier::Status::Normal: {
        break;
      }
      }
    }
  }

  // for consumer
  template <typename NotifierType, typename CallBackWhenEmpty>
  Element Get(NotifierType &&consumer_notifier, CallBackWhenEmpty &&cb_empty) {
    for (;;) {
      if (auto res = TryGet(); res) {
        return std::move(*res);
      } else {
        cb_empty();
      }
      consumer_notifier.BlockedWait();
    }
  }

  // for consumer
  std::optional<Element> TryGet() { return spsc_queue_.Get(); }

  template <typename... Args>
  SPSCQueueWithNotifer(Args &&...args)
      : spsc_queue_(std::forward<Args>(args)...) {}

  const ProducerNotifier &producer_notifier() const {
    return producer_notifier_;
  }

private:
  SPSC spsc_queue_;
  ProducerNotifier producer_notifier_;
};

} // namespace utils
