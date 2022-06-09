#pragma once

#include <cassert>
#include <optional>
#include <thread>
#include <utility>

template <typename T, typename Allocator = std::allocator<T>> struct SPSCQueue {
  static uint32_t NextPow2(uint32_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
  }
  static size_t CalcMaxNum(size_t cap) {
    assert(cap < std::numeric_limits<uint32_t>::max());
    if (cap < 2)
      return 2;
    return NextPow2(cap);
  }

  explicit SPSCQueue(uint32_t cap, const Allocator &allocator = Allocator())
      : allocator_(allocator), max_num_(CalcMaxNum(cap + 1)),
        buffer_mask_(max_num_ - 1), buff_(max_num_, allocator_) {}

  SPSCQueue(const SPSCQueue &) = delete;

  struct Buff {
    explicit Buff(size_t n, Allocator &allocator) {
      inner_ = std::allocator_traits<Allocator>::allocate(allocator, n);
    }
    const T &operator[](size_t idx) const { return inner_[idx]; }
    void set(size_t idx, T &&v) { new (inner_ + idx) T(std::forward<T>(v)); }
    template <typename F> void pop_impl(size_t idx, F &&f) {
      static_assert(std::is_destructible_v<T>);
      f(inner_[idx]);
      inner_[idx].~T();
    }
    void pop(size_t idx, T &v) {
      pop_impl(idx, [&](T &e) { v = std::move(e); });
    }
    void pop(size_t idx) {
      pop_impl(idx, [&](T &) {});
    }
    void clear(size_t n, Allocator &allocator) {
      if (!inner_)
        return;
      std::allocator_traits<Allocator>::deallocate(allocator, inner_, n);
      inner_ = nullptr;
    }

  private:
    T *inner_;
  };

  ~SPSCQueue() {
    while (!isEmpty()) {
      auto cur = head_.load(std::memory_order_relaxed);
      buff_.pop(cur);
      head_.store((cur + 1) & buffer_mask_, std::memory_order_release);
    }
    buff_.clear(max_num_, allocator_);
  }

  static constexpr size_t CPU_CACHE_LINE_SIZE = 64;
  alignas(CPU_CACHE_LINE_SIZE) std::atomic_size_t head_{};
  alignas(CPU_CACHE_LINE_SIZE) std::atomic_size_t tail_{};
  alignas(CPU_CACHE_LINE_SIZE) size_t head_cache_{};
  alignas(CPU_CACHE_LINE_SIZE) size_t tail_cache_{};
  [[maybe_unused]] alignas(CPU_CACHE_LINE_SIZE) uint8_t pad_{};
  Allocator allocator_;
  const size_t max_num_;
  const size_t buffer_mask_;
  Buff buff_;

  bool put(T &&data) {
    if (isFull())
      return false;
    auto cur = tail_.load(std::memory_order_relaxed);
    auto next = (cur + 1) & buffer_mask_;
    buff_.set(cur, std::move(data));
    tail_.store(next, std::memory_order_release);
    return true;
  }

  bool isEmpty() {
    auto cur = head_.load(std::memory_order_relaxed);
    if (cur == tail_cache_) {
      tail_cache_ = tail_.load(std::memory_order_acquire);
      if (cur == tail_cache_)
        return true;
    }
    return false;
  }

  bool isFull() {
    auto cur = tail_.load(std::memory_order_relaxed);
    auto next = (cur + 1) & buffer_mask_;
    if (next == head_cache_) {
      head_cache_ = head_.load(std::memory_order_acquire);
      if (next == head_cache_) {
        return true;
      }
    }
    return false;
  }

  std::optional<T> get() {
    if (isEmpty())
      return {};
    std::optional<T> res{};
    get_impl([&](T &v) { res = std::move(v); });
    return res;
  }

  bool get(T &res) {
    if (isEmpty())
      return false;
    get_impl([&](T &v) { res = std::move(v); });
    return true;
  }

  static const char *name() { return "SPSCQueue"; }

private:
  template <typename F> void get_impl(F &&f) {
    auto cur = head_.load(std::memory_order_relaxed);
    buff_.pop_impl(cur, std::forward<F>(f));
    head_.store((cur + 1) & buffer_mask_, std::memory_order_release);
  }
};

static void _test_spsc_queue() {
  {
    SPSCQueue<uint8_t> s(1);
    assert(s.put(88));
    assert(s.get().value() == 88);
  }
  {
    SPSCQueue<std::string> s(13);
    assert(s.max_num_ == 16);
    {
      std::string ori = "12345";
      assert(s.put(std::string(ori)));
      auto v = s.get();
      assert(v);
      assert((*v) == ori);
      assert(!s.get());
    }
  }
  {
    SPSCQueue<std::string> s(3);
    assert(s.max_num_ == 4);
    for (int i = 0; i < 3; ++i) {
      assert(s.put(std::to_string(i)));
    }
    assert(!s.put("..."));
    std::string v;
    assert(s.get(v));
    assert(v == "0");
    assert(s.get(v));
    assert(v == "1");
    assert(s.get(v));
    assert(v == "2");
    {
      assert(s.put("..."));
      assert(s.get() == "...");
    }
  }
  {
    static std::atomic<int> cnt = 0;
    struct Test {
      Test(const Test &) = delete;
      Test(int g) : v(g) { inc_cnt(); }
      ~Test() {
        if (v)
          dec_cnt();
      }
      Test(Test &&src) { std::swap(src.v, v); }
      Test &operator=(Test &&src) {
        std::swap(src.v, v);
        return *this;
      }
      static void inc_cnt() { cnt++; }
      static void dec_cnt() { cnt--; }
      int v{};
    };
    {
      Test _{1};
      assert(cnt == 1);
    }
    assert(cnt == 0);
    {
      SPSCQueue<Test> s(3);
      s.put(Test{2});
      s.put(Test{3});
      s.put(Test{4});
      assert(cnt == 3);
      s.get();
      assert(cnt == 2);
    }
    assert(cnt == 0);

    {
      SPSCQueue<Test> s(3);
      std::thread t1([&]() {
        for (int i = 0; i < 8192; ++i) {
          while (!s.put(Test{i + 1}))
            std::this_thread::yield();
        }
      });
      for (int i = 0; i < 8192; ++i) {
        while (1) {
          auto v = s.get();
          if (v) {
            assert(v->v == i + 1);
            break;
          } else
            std::this_thread::yield();
        }
      }
      t1.join();
      assert(cnt == 0);
    }
  }
}