#pragma once

#include <utils/utils.h>

namespace utils {

/**
  head  ->  ... ->  tail  -> ... ->  head
  get()             put()

  head == tail   -> empty
  head == 1+tail -> full
*/
template <typename T, typename Allocator = std::allocator<T>> struct SPSCQueue {
  using Element = T;

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

  inline size_t next_pos(size_t pos) const { return (pos + 1) & buffer_mask_; }

  ~SPSCQueue() {
    for (size_t cur = head(); cur != tail(); cur = next_pos(cur)) {
      GetAtImpl(cur, [&](T &) constexpr {});
    }
    head() = tail() = 0;
    buff_.Deallocate(max_num_, allocator_);
  }

  void Format(fmt::memory_buffer &out) const {
    bool is_first = true;
    for (size_t cur = head(); cur != tail(); cur = next_pos(cur)) {
      FMT_IF_APPEND(out, !is_first, ", ");
      FMT_APPEND(out, "`{}`", buff_[cur]);
      is_first = false;
    }
  }

  static const char *name() { return "SPSCQueue"; }

  size_t capacity() const { return max_num_ - 1; }

  static constexpr size_t npos = -1;

  // for consumer
  size_t ConsumerCheckEmpty() {
    const auto _head = head().load(std::memory_order_relaxed);
    if (_head == tail_cache_for_get()) {
      tail_cache_for_get() = latest_tail();
      if (_head == tail_cache_for_get())
        return npos;
    }
    return _head;
  }

  // for producer
  size_t ProducerCheckFull() {
    const auto _tail = tail().load(std::memory_order_relaxed);
    const auto next = next_pos(_tail);
    if (next == head_cache_for_put()) {
      head_cache_for_put() = latest_head();
      if (next == head_cache_for_put()) {
        return npos;
      }
    }
    return _tail;
  }

  size_t latest_head() const { return head().load(std::memory_order_acquire); }
  size_t latest_tail() const { return tail().load(std::memory_order_acquire); }

  // consumer gets from head
  std::optional<T> Get() {
    const size_t pos = ConsumerCheckEmpty();
    if (pos == npos) {
      return std::nullopt;
    }
    std::optional<T> res{};
    GetAtImpl(pos, [&](T &v) { res.emplace(std::move(v)); });
    PublishHead(next_pos(pos));
    return res;
  }

  bool Get(T &res) {
    const size_t pos = ConsumerCheckEmpty();
    if (pos == npos) {
      return false;
    }
    GetAtImpl(pos, [&](T &v) { res = std::move(v); });
    PublishHead(next_pos(pos));
    return true;
  }

  // producer puts to tail
  bool Put(T &&data) {
    const size_t pos = ProducerCheckFull();
    if (pos == npos) {
      return false;
    }
    buff_.New(pos, std::move(data));
    PublishTail(next_pos(pos));
    return true;
  }

private:
  template <typename F> void GetAtImpl(size_t index, F &&f) {
    static_assert(std::is_destructible_v<T>);
    f(buff_[index]);
    buff_.Del(index);
  }

#define IndexCache(name, self, tar)                                            \
  struct IndexCache##name {                                                    \
    std::atomic_size_t self;                                                   \
    size_t tar##_cache;                                                        \
  }

  IndexCache(Get, head, tail);
  IndexCache(Put, tail, head);

#undef IndexCache

  std::atomic_size_t &head() { return get_->head; }
  const std::atomic_size_t &head() const { return get_->head; }
  std::atomic_size_t &tail() { return put_->tail; }
  const std::atomic_size_t &tail() const { return put_->tail; }
  size_t &head_cache_for_put() { return put_->head_cache; }
  const size_t &head_cache_for_put() const { return put_->head_cache; }
  size_t &tail_cache_for_get() { return get_->tail_cache; }
  const size_t &tail_cache_for_get() const { return get_->tail_cache; }
  void PublishTail(size_t pos) { tail().store(pos, std::memory_order_release); }
  void PublishHead(size_t pos) { head().store(pos, std::memory_order_release); }

private:
  [[no_unique_address]] Allocator allocator_;
  const size_t max_num_;
  const size_t buffer_mask_;
  ConstSizeArray<T, Allocator> buff_;
  // Aligned to CPU cache line size
  // consumer get
  CPUCacheLineAligned<IndexCacheGet> get_;
  // producer put
  CPUCacheLineAligned<IndexCachePut> put_;
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
