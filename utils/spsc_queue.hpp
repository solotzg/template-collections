#pragma once

#include <cassert>
#include <cstdint>
#include <optional>
#include <thread>
#include <utility>

template <size_t CpuCacheLineSize = 64> struct SPSCQueueBase {
  static constexpr size_t CPU_CACHE_LINE_SIZE = CpuCacheLineSize;
};

template <typename T, typename Allocator = std::allocator<T>,
          typename Base = SPSCQueueBase<>>
struct SPSCQueue : Base {
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
    void Set(size_t idx, T &&v) { new (inner_ + idx) T(std::forward<T>(v)); }
    template <typename F> void PopImpl(size_t idx, F &&f) {
      static_assert(std::is_destructible_v<T>);
      f(inner_[idx]);
      inner_[idx].~T();
    }
    void Pop(size_t idx, T &v) {
      PopImpl(idx, [&](T &e) { v = std::move(e); });
    }
    void Pop(size_t idx) {
      PopImpl(idx, [&](T &) {});
    }
    void Clear(size_t n, Allocator &allocator) {
      if (!inner_)
        return;
      std::allocator_traits<Allocator>::deallocate(allocator, inner_, n);
      inner_ = nullptr;
    }

  private:
    T *inner_;
  };

  ~SPSCQueue() {
    while (!IsEmpty()) {
      auto cur = head_.load(std::memory_order_relaxed);
      buff_.Pop(cur);
      head_.store((cur + 1) & buffer_mask_, std::memory_order_release);
    }
    buff_.Clear(max_num_, allocator_);
  }

  bool Put(T &&data) {
    if (IsFull())
      return false;
    auto cur = tail_.load(std::memory_order_relaxed);
    auto next = (cur + 1) & buffer_mask_;
    buff_.Set(cur, std::move(data));
    tail_.store(next, std::memory_order_release);
    return true;
  }

  bool IsEmpty() {
    auto cur = head_.load(std::memory_order_relaxed);
    if (cur == tail_cache_) {
      tail_cache_ = tail_.load(std::memory_order_acquire);
      if (cur == tail_cache_)
        return true;
    }
    return false;
  }

  bool IsFull() {
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

  std::optional<T> Get() {
    if (IsEmpty())
      return {};
    std::optional<T> res{};
    GetImpl([&](T &v) { res = std::move(v); });
    return res;
  }

  bool Get(T &res) {
    if (IsEmpty())
      return false;
    GetImpl([&](T &v) { res = std::move(v); });
    return true;
  }

  static const char *name() { return "SPSCQueue"; }

  size_t capacity() const { return max_num_ - 1; }

private:
  template <typename F> void GetImpl(F &&f) {
    auto cur = head_.load(std::memory_order_relaxed);
    buff_.PopImpl(cur, std::move(f));
    head_.store((cur + 1) & buffer_mask_, std::memory_order_release);
  }

private:
  alignas(Base::CPU_CACHE_LINE_SIZE) std::atomic_size_t head_{};
  alignas(Base::CPU_CACHE_LINE_SIZE) std::atomic_size_t tail_{};
  alignas(Base::CPU_CACHE_LINE_SIZE) size_t head_cache_{};
  alignas(Base::CPU_CACHE_LINE_SIZE) size_t tail_cache_{};
  [[maybe_unused]] alignas(Base::CPU_CACHE_LINE_SIZE) uint8_t pad_{};
  Allocator allocator_;
  const size_t max_num_;
  const size_t buffer_mask_;
  Buff buff_;
};

#ifndef NDEBUG

static void _test_spsc_queue() {
  static_assert(alignof(SPSCQueue<void *>) == 64);
  {
    SPSCQueue<uint8_t> s(1);
    assert(s.Put(88));
    assert(s.Get().value() == 88);
  }
  {
    SPSCQueue<std::string> s(13);
    assert(s.capacity() == 15);
    {
      std::string ori = "12345";
      assert(s.Put(std::string(ori)));
      auto v = s.Get();
      assert(v);
      assert((*v) == ori);
      assert(!s.Get());
    }
  }
  {
    SPSCQueue<std::string> s(3);
    assert(s.capacity() == 3);
    for (int i = 0; i < 3; ++i) {
      assert(s.Put(std::to_string(i)));
    }
    assert(!s.Put("..."));
    std::string v;
    assert(s.Get(v));
    assert(v == "0");
    assert(s.Get(v));
    assert(v == "1");
    assert(s.Get(v));
    assert(v == "2");
    {
      assert(s.Put("..."));
      assert(s.Get() == "...");
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
      s.Put(Test{2});
      s.Put(Test{3});
      s.Put(Test{4});
      assert(cnt == 3);
      s.Get();
      assert(cnt == 2);
    }
    assert(cnt == 0);

    {
      SPSCQueue<Test> s(3);
      std::thread t1([&]() {
        for (int i = 0; i < 8192; ++i) {
          while (!s.Put(Test{i + 1}))
            std::this_thread::yield();
        }
      });
      for (int i = 0; i < 8192; ++i) {
        while (1) {
          auto v = s.Get();
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

#endif