#pragma once

#include "spsc_queue.hpp"
#include "utils/list-def.h"

namespace utils {

/*
enum class Priority : size_t { High = 0, Mid, Low, _MAX };

static std::string_view PriorityName(Priority p) {
  static constexpr std::string_view names[] = {
      "High",
      "Mid",
      "Low",
  };
  static_assert(std::size(names) == size_t(Priority::_MAX));
  return names[size_t(p)];
}

struct MPSCWorkerConfig {
  static constexpr size_t Cnt = static_cast<size_t>(Priority::_MAX);
  const std::array<size_t, Cnt> queue_size_;
  const std::array<size_t, Cnt> queue_cap_;
};

struct size_t : std::array<size_t, MPSCWorkerConfig::Cnt> {};

static std::ostream &operator<<(std::ostream &ostr, const size_t &node) {
  ostr << fmt::format("[{}]", fmt::join(node, ","));
  return ostr;
}
*/

template <typename T, typename Allocator = std::allocator<T>,
          typename AllocatorSPSC = std::allocator<SPSCQueue<T, Allocator>>>
struct MPSCWorker {
  using Element = T;
  using SPSCQueueType = SPSCQueue<T, Allocator>;
  using String = std::string;

  explicit MPSCWorker(size_t producer_size, size_t queue_cap,
                      const Allocator &allocator = Allocator(),
                      const AllocatorSPSC &spsc_allocator = AllocatorSPSC())
      : producer_size_(producer_size), spsc_allocator_(spsc_allocator) {
    spsc_queues_.Allocate(producer_size_, spsc_allocator_);
    rp(pos, producer_size_) spsc_queues_.New(pos, queue_cap, allocator);
  }

  size_t producer_size() const { return producer_size_; }

  ~MPSCWorker() {
    rp(pos, producer_size_) spsc_queues_.Del(pos);
    spsc_queues_.Deallocate(producer_size_, spsc_allocator_);
  }

  // unsafe
  // If `pos_hint` is same, `data` will be put into same FIFO channel.
  bool Put(T &&data, size_t pos_hint) {
    size_t pos = pos_hint % producer_size_;
    return spsc_queues_[pos].GenProducer().Put(std::move(data));
  }
  // unsafe
  // `index` must be LT queue size
  bool PutAt(T &&data, size_t index) {
    ASSERT_LT(index, producer_size_);
    return spsc_queues_[index].GenProducer().Put(std::move(data));
  }

  String Show() const {
    fmt::memory_buffer out{};
    Format(out);
    return fmt::to_string(out);
  }
  void Format(fmt::memory_buffer &out) const {
    SCOPE_EXIT_FMT_SURROUND(out, "{", "}");
    FMT_APPEND(out, "size:{}", producer_size_);
    FMT_APPEND(out, ", data:");
    SCOPE_EXIT_FMT_SURROUND(out, "{", "}");
    {
      for (size_t i = 0; i < producer_size_; ++i) {
        FMT_IF_APPEND(out, i, ", ");
        FMT_APPEND(out, "queue_{}:", i);
        SCOPE_EXIT_FMT_SURROUND(out, "{", "}");
        auto &spsc = spsc_queues_[i];
        spsc.Format(out);
      }
    }
  }

  struct MPSCWorkerCustomer {
    template <typename CallBack, typename CallBackWhenEmpty>
    size_t Consume(CallBack &&cb, CallBackWhenEmpty &&cb_empty,
                   const size_t max_consume_cnt = 8) {
      return worker_.Consume(cb, cb_empty, max_consume_cnt);
    }

    MPSCWorkerCustomer(MPSCWorker &worker) : worker_(worker) {}

  private:
    friend struct MPSCWorker;
    MPSCWorkerCustomer(const MPSCWorkerCustomer &) = delete;
    MPSCWorker &worker_;
  };

  MPSCWorkerCustomer GenCustomer() { return MPSCWorkerCustomer{*this}; }

protected:
  // unsafe
  template <typename CallBack, typename CallBackWhenEmpty>
  size_t Consume(CallBack &&cb, CallBackWhenEmpty &&cb_empty,
                 const size_t max_consume_cnt) {
    size_t consume_cnt{};

    for (;;) {
      bool any_consumed = false;
      const auto ori_queue_index = queue_round_robin_index();

      auto &&func_loop = [&](const size_t end) {
        for (; queue_round_robin_index() < end && consume_cnt < max_consume_cnt;
             ++queue_round_robin_index()) {
          auto &spsc = spsc_queues_[queue_round_robin_index()];
          if (auto t = spsc.GenCustomer().Get(); t) {
            cb(std::move(*t), queue_round_robin_index());
            consume_cnt++;
            any_consumed = true;
          } else {
            cb_empty(queue_round_robin_index());
          }
        }
      };

      {
        // round robin start ~ end
        func_loop(producer_size_);
      }
      if (consume_cnt < max_consume_cnt) {
        // 0 ~ round robin start
        queue_round_robin_index() = 0;
        func_loop(ori_queue_index);
      }

      if (!any_consumed) {
        break;
      }
    }

    return consume_cnt;
  }

private:
  MPSCWorker(const MPSCWorker &) = delete;
  size_t &queue_round_robin_index() { return *queue_round_robin_index_; }
  const size_t &queue_round_robin_index() const {
    return *queue_round_robin_index_;
  }

private:
  const size_t producer_size_;
  AllocatorSPSC spsc_allocator_;
  ConstSizeArray<SPSCQueueType, AllocatorSPSC> spsc_queues_;
  // writable
  CPUCacheLineAligned<size_t> queue_round_robin_index_{};
};

template <typename MPSC> struct MPSCQueueWithNotifer {
  using Element = typename MPSC::Element;
  using Duration = Milliseconds;

  template <typename CB> bool Put(Element &&e, size_t pos, CB &&cb_when_full) {
    for (;;) {
      if (auto res = TryPut(std::forward<Element>(e), pos); res) {
        return res;
      } else {
        cb_when_full();
      }
      producer_notifier(pos).BlockedWait();
    }
  }

  template <typename CB>
  bool Put(Element &&e, size_t pos, const Duration &timeout,
           CB &&cb_when_full) {
    if (auto res = TryPut(std::forward<Element>(e), pos); res) {
      return res;
    }
    auto time_point = SteadyClock::now() + timeout;
    return Put(std::forward<Element>(e), pos, time_point,
               std::move(cb_when_full));
  }
  template <typename CB>
  bool Put(Element &&e, size_t pos, const SteadyClock::time_point &time_point,
           CB &&cb_when_full) {
    for (;;) {
      if (auto res = TryPut(std::forward<Element>(e), pos); res) {
        return res;
      } else {
        cb_when_full();
      }
      switch (producer_notifier(pos).BlockedWaitUtil(time_point)) {
      case Notifier::Status::Timeout:
        return false;
      case Notifier::Status::Normal:
        break;
      }
    }
  }

  bool TryPut(Element &&e, size_t pos) {
    return mpsc_queue_.PutAt(std::forward<Element>(e), pos);
  }

  template <typename CallBack, typename CallBackWhenEmpty,
            typename NotifierType>
  size_t Get(CallBack &&cb, CallBackWhenEmpty &&cb_empty,
             const Duration &timeout, const size_t max_consume_cnt,
             NotifierType &&notifier) {
    if (auto res = TryGet(cb, cb_empty, max_consume_cnt); res) {
      return res;
    }
    auto time_point = SteadyClock::now() + timeout;
    return Get(cb, cb_empty, time_point, max_consume_cnt,
               std::forward<NotifierType>(notifier));
  }

  template <typename CallBack, typename CallBackWhenEmpty,
            typename NotifierType>
  size_t Get(CallBack &&cb, CallBackWhenEmpty &&cb_empty,
             const SteadyClock::time_point &time_point,
             const size_t max_consume_cnt, NotifierType &&notifier) {
    for (;;) {
      if (auto res = TryGet(cb, cb_empty, max_consume_cnt); res) {
        return res;
      }
      switch (notifier.BlockedWaitUtil(time_point)) {
      case Notifier::Status::Timeout:
        return 0;
      case Notifier::Status::Normal:
        break;
      }
    }
  }

  template <typename CallBack, typename CallBackWhenEmpty,
            typename NotifierType>
  size_t Get(CallBack &&cb, CallBackWhenEmpty &&cb_empty,
             const size_t max_consume_cnt, NotifierType &&notifier) {
    for (;;) {
      if (auto res = TryGet(cb, cb_empty, max_consume_cnt); res) {
        return res;
      }
      notifier.BlockedWait();
    }
  }

  template <typename CallBack, typename CallBackWhenEmpty>
  size_t TryGet(CallBack &&cb, CallBackWhenEmpty &&cb_empty,
                const size_t max_consume_cnt) {
    return mpsc_queue_.GenCustomer().Consume(cb, cb_empty, max_consume_cnt);
  }

  void WakeProducer(size_t pos) const { producer_notifier(pos).Wake(); }
  void WakeAllProducers() const {
    for (size_t i = 0; i < producer_size(); ++i)
      producer_notifier(i).Wake();
  }

  template <typename... Args>
  static MPSCQueueWithNotifer NewMPSCQueueWithNotifer(Args &&...args) {
    return MPSCQueueWithNotifer(std::forward<Args>(args)...);
  }

  ~MPSCQueueWithNotifer() {
    rp(i, producer_size()) producer_notifier_.Del(i);
    producer_notifier_.Deallocate(producer_size());
  }

  typename MPSC::String Show() const { return mpsc_queue_.Show(); }

  const Notifier &producer_notifier(size_t i) const {
    return producer_notifier_[i];
  }

  template <typename... Args>
  MPSCQueueWithNotifer(Args &&...args)
      : mpsc_queue_(std::forward<Args>(args)...) {
    producer_notifier_.Allocate(producer_size());
    for (size_t i = 0; i < producer_size(); ++i)
      producer_notifier_.New(i);
  }

  size_t producer_size() const { return mpsc_queue_.producer_size(); }

private:
  Notifier &producer_notifier(size_t i) { return producer_notifier_[i]; }

private:
  MPSC mpsc_queue_;
  ConstSizeArray<Notifier> producer_notifier_;
};

template <typename T> struct FastAlloc : utils::SpinLockWrap {
  using Self = FastAlloc<T>;
  struct DataHolder : utils::noncopyable {

    DataHolder(T *ptr, FastAlloc &allocator)
        : ptr_(ptr), allocator_(allocator) {
      __builtin_assume(ptr_);
    }

    ~DataHolder() {
      if (ptr_)
        allocator_.Dealloc(ptr_);
    }

    T *TakeData() { return std::exchange(ptr_, nullptr); }

  private:
    T *ptr_;
    FastAlloc &allocator_;
  };

  DataHolder Alloc() {
    return RunWithLock([&] { return AllocImpl(); });
  }

protected:
  void Dealloc(T *ptr) {
    RunWithLock([&] { fast_bin_.Dealloc(ptr); });
  }

  DataHolder AllocImpl() {
    auto data_ptr = static_cast<T *>(fast_bin_.Alloc());
    return DataHolder{data_ptr, *this};
  }

protected:
  utils::FastBin<T> fast_bin_;
};

template <typename T> struct MPSCQueueFastBinAlloc {
  struct Node;
  using FastAlloc = FastAlloc<Node>;

  struct Node : utils::noncopyable {
    template <typename... Args>
    Node(Args &&...args) : data_(std::forward<Args>(args)...) {
      head_.next = head_.prev = nullptr;
    }

    utils::ListHead *&mut_next(utils::ListHead *tar) {
      return head_.next = tar;
    }

    static Node *ContainerOf(utils::ListHead *ptr) {
      return inner_containerof(ptr, Node, head_);
    }

    utils::ListHead *ptr() { return &head_; }

    T &data() { return data_; }

  private:
    T data_;
    utils::ListHead head_;
  };

  struct SPSC {

    template <typename... Args>
    void Push(std::atomic_size_t &size, FastAlloc &fast_alloc,
              Args &&...args) noexcept {
      auto ptr = fast_alloc.Alloc().TakeData();
      Node *data = new (ptr) Node(std::forward<Args>(args)...);
      utils::ListHead *ori_tail = tail();
      data->mut_next(ori_tail);
      for (; !tail().compare_exchange_strong(ori_tail, data->ptr());) {
        std::this_thread::yield();
        ori_tail = tail();
        data->mut_next(ori_tail);
      }
      size += 1;
    }

    bool TryGenQueue() {
      utils::ListHead *ori_tail = tail().load(std::memory_order_acquire);
      if (!ori_tail)
        return false;

      ori_tail = tail().exchange(nullptr);

      utils::ListHead head;
      {
        head.next = ori_tail;
        utils::ListHead *cur = &head, *next = head.next;
        for (; next;) {
          utils::ListHead *tmp = next->next;
          cur->prev = next;
          next->next = cur;

          cur = next;
          next = tmp;
        }
        cur->prev = &head;
        cur->prev->next = cur;
      }
      inner_list_splice_tail(&head, &queue_head());
      return true;
    }

    template <typename F> bool Consume(FastAlloc &fast_alloc, F &&f) {
      if (inner_list_is_empty(&queue_head())) {
        TryGenQueue();
      }
      if (inner_list_is_empty(&queue_head()))
        return false;
      {
        auto *first_node_head_ptr = queue_head().next;
        inner_list_del(first_node_head_ptr);
        DataHolder holder(Node::ContainerOf(first_node_head_ptr), fast_alloc);
        f(holder.data());
      }
      return true;
    }

    SPSC() { inner_list_init(&queue_head()); }
    utils::ListHead &queue_head() { return *queue_head_; }
    std::atomic<utils::ListHead *> &tail() { return *tail_; }

    utils::CPUCacheLineAligned<std::atomic<utils::ListHead *>> tail_{};
    utils::CPUCacheLineAligned<utils::ListHead> queue_head_{};
  };

  template <typename... Args>
  bool TryPush(size_t index, Args &&...args) noexcept {
    if (IsFull())
      return false;
    (*spsc_queues_)[index].Push(*size_, fast_alloc(),
                                std::forward<Args>(args)...);
    return true;
  }

  template <typename F>
  size_t
  Consume(F &&f,
          const size_t max_consume_cnt = std::numeric_limits<size_t>::max()) {
    size_t consume_cnt{};

    for (;;) {
      bool any_consumed = false;
      const auto ori_queue_index = queue_round_robin_index();

      auto &&func_loop = [&](const size_t end) {
        for (; queue_round_robin_index() < end && consume_cnt < max_consume_cnt;
             ++queue_round_robin_index()) {
          auto &spsc = spsc_queues()[queue_round_robin_index()];
          if (spsc.Consume(fast_alloc(), std::forward<F>(f))) {
            consume_cnt++;
            any_consumed = true;
          }
        }
      };

      {
        // round robin start ~ end
        func_loop(producer_size_);
      }
      if (consume_cnt < max_consume_cnt) {
        // 0 ~ round robin start
        queue_round_robin_index() = 0;
        func_loop(ori_queue_index);
      }

      if (!any_consumed) {
        break;
      }
    }

    size_->fetch_sub(consume_cnt, std::memory_order_release);
    return consume_cnt;
  }

  MPSCQueueFastBinAlloc(size_t producer_size,
                        size_t limit = std::numeric_limits<size_t>::max())
      : producer_size_(producer_size), size_limit_(limit) {
    spsc_queues_->Allocate(producer_size_);
    rp(i, producer_size_) spsc_queues_->New(i);
  }
  ~MPSCQueueFastBinAlloc() {
    rp(i, producer_size_) spsc_queues_->Del(i);
    spsc_queues_->Deallocate(producer_size_);
  }

  struct DataHolder : utils::noncopyable {
    DataHolder(Node *entry, FastAlloc &allocator)
        : entry_(entry), allocator_(allocator) {}
    ~DataHolder() {
      entry_->~Node();
      typename FastAlloc::DataHolder holder(entry_, allocator_);
    }
    T &data() { return entry_->data(); }

  private:
    Node *entry_;
    FastAlloc &allocator_;
  };

  bool IsFull() const { return size() >= size_limit_; }
  size_t size() const { return size_->load(std::memory_order_relaxed); }

protected:
  FastAlloc &fast_alloc() { return *fast_alloc_; }
  size_t &queue_round_robin_index() { return *queue_round_robin_index_; }
  const size_t &queue_round_robin_index() const {
    return *queue_round_robin_index_;
  }
  ConstSizeArray<SPSC> &spsc_queues() { return *spsc_queues_; }

protected:
  const size_t producer_size_;
  const size_t size_limit_;
  CPUCacheLineAligned<std::atomic_size_t> size_{};
  CPUCacheLineAligned<FastAlloc> fast_alloc_;
  CPUCacheLineAligned<ConstSizeArray<SPSC>> spsc_queues_;
  CPUCacheLineAligned<size_t> queue_round_robin_index_{};
};

} // namespace utils
