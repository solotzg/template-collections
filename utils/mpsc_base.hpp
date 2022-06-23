#pragma once

#include "spsc_queue.hpp"
#include "timer.hpp"
#include "utils.hpp"

#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <limits>
#include <unordered_map>
#include <utility>

enum class Priority : size_t { High = 0, Mid, Low, _MAX };

struct MPSCWorkerConfig {
  static constexpr size_t Cnt = static_cast<size_t>(Priority::_MAX);
  const std::array<size_t, Cnt> queue_size_;
  const std::array<size_t, Cnt> queue_cap_;
};

using MPSCCntNode = std::array<size_t, MPSCWorkerConfig::Cnt>;

template <typename T, typename Allocator = std::allocator<T>,
          typename AllocatorSPSC = std::allocator<SPSCQueue<T, Allocator>>>
struct MPSCWorker {
  using SPSCQueueType = SPSCQueue<T, Allocator>;
  explicit MPSCWorker(MPSCWorkerConfig config,
                      const Allocator &allocator = Allocator(),
                      const AllocatorSPSC &spsc_allocator = AllocatorSPSC())
      : config_(config), producer_index_start_({
                             0,
                             config_.queue_size_[0],
                             config_.queue_size_[0] + config_.queue_size_[1],
                         }),
        spsc_allocator_(spsc_allocator) {

    {
      auto producer_size = std::accumulate(config_.queue_size_.begin(),
                                           config_.queue_size_.end(), 0);
      spsc_queues_ = std::allocator_traits<AllocatorSPSC>::allocate(
          spsc_allocator_, producer_size);
    }

    for (size_t i = 0, start = 0, end = 0; i != config_.Cnt; ++i) {
      end = start + config_.queue_size_[i];
      for (size_t pos = start; pos != end; ++pos) {
        new (spsc_queues_ + pos)
            SPSCQueueType(config_.queue_cap_[i], allocator);
      }
      start += config_.queue_size_[i];
    }
  }

  ~MPSCWorker() {
    auto producer_size = std::accumulate(config_.queue_size_.begin(),
                                         config_.queue_size_.end(), 0);

    for (size_t i = 0; i < producer_size; ++i) {
      spsc_queues_[i].~SPSCQueueType();
    }
    std::allocator_traits<AllocatorSPSC>::deallocate(
        spsc_allocator_, spsc_queues_, producer_size);
  }

  // unsafe
  // If `pos_hint` is same, `data` will be put into same FIFO channel.
  template <Priority priority = Priority::High>
  bool Put(T &&data, size_t pos_hint) {
    size_t pos =
        GetIndexStart<priority>() + pos_hint % GetIndexSize<priority>();
    return spsc_queues_[pos].GenProducer().Put(std::move(data));
  }

  struct MPSCWorkerCustomer {
    template <typename CB>
    MPSCCntNode Consume(CB &&cb,
                        const MPSCCntNode &consume_config = {8, 8, 8}) {
      return worker_.Consume(std::move(cb), consume_config);
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
  template <typename CB>
  MPSCCntNode Consume(CB &&cb, const MPSCCntNode &consume_config) {
    MPSCCntNode consume_cnt{0, 0, 0};

    for (bool stop = false; !stop; stop = true) {
      for (size_t index = 0, start = 0, end = 0; index != config_.Cnt;
           ++index) {
        end = start + config_.queue_size_[index];
        for (;;) {
          auto ori = consume_cnt[index];
          for (size_t pos = start;
               pos != end && consume_cnt[index] < consume_config[index];
               ++pos) {
            auto &spsc = spsc_queues_[pos];
            if (auto t = spsc.GenCustomer().Get(); t) {
              cb(std::move(*t));
              consume_cnt[index]++;
            }
          }
          if (ori == consume_cnt[index])
            break;
        }
        start += config_.queue_size_[index];
      }
    }

    return consume_cnt;
  }

private:
  MPSCWorker(const MPSCWorker &) = delete;

  template <Priority priority> const size_t &GetIndexStart() const {
    return producer_index_start_[static_cast<size_t>(priority)];
  }

  template <Priority priority> const size_t &GetIndexSize() const {
    return config_.queue_size_[static_cast<size_t>(priority)];
  }

private:
  const MPSCWorkerConfig config_;
  const MPSCCntNode producer_index_start_;
  AllocatorSPSC spsc_allocator_;
  SPSCQueueType *spsc_queues_;
};

#ifndef NDEBUG

static void _test_mpsc_queue() {
  {
    MPSCWorker<int> mpsc({{2, 3, 1}, {3, 1, 1}});
    //
    assert(mpsc.Put(1, 0));
    assert(mpsc.Put(2, 1));
    assert(mpsc.Put(3, 2));
    assert(mpsc.Put(4, 1));
    assert(mpsc.Put(5, 1));
    assert(!mpsc.Put(6, 1));
    assert(mpsc.Put<Priority::Low>(6, 1024));
    {
      auto ex = -1;
      auto r = mpsc.GenCustomer().Consume([&](int x) { ex = x; }, {1});
      assert(std::accumulate(r.begin(), r.end(), 0) == 1);
      assert(ex == 1);
    }

    {
      std::set<int> data;
      auto r = mpsc.GenCustomer().Consume(
          [&](int x) { data.emplace(x); },
          {0, 0, std ::numeric_limits<size_t>::max()});
      assert((r == MPSCCntNode{0, 0, 1}));
      assert(data == std::set<int>{6});
    }

    {
      std::vector<int> data;
      auto r =
          mpsc.GenCustomer().Consume([&](int x) { data.emplace_back(x); },
                                     {std ::numeric_limits<size_t>::max(),
                                      std ::numeric_limits<size_t>::max(),
                                      std ::numeric_limits<size_t>::max()});
      assert((r == MPSCCntNode{4, 0, 0}));
      assert((data == std::vector{3, 2, 4, 5}));
    }
    {}
  }
}

#endif