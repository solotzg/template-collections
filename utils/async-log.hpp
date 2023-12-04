#pragma once

#include <utils/async.hpp>

namespace utils {

template <typename SystemClockType = SystemClock,
          typename SteadyClockType = SteadyClock>
struct UniqAsyncLog : MutexLockWrap {
  using Message = std::string;
  using TimePoint = typename SystemClockType::time_point;
  using Self = UniqAsyncLog;
  using Element = size_t;
  using SPSC = SPSCQueueWithNotifer<SPSCQueue<Element>, utils::AtomicNotifier>;
  using ProducerLock = MutexLockWrap;

  struct BufferCache : utils::noncopyable {
    enum class Status {
      None,
      Waiting,
      Ok,
    };

    using Available = std::atomic_bool;
    using Flag = std::atomic<Status>;
    using AvailableCnt = std::atomic_int32_t;

    BufferCache(size_t n) : size_(n) {
      cache_.Allocate(size());
      rp(i, size()) {
        cache_.New(i);
        release(i);
      }
    }

    ~BufferCache() {
      rp(i, size()) { cache_.Del(i); }
      cache_.Deallocate(size());
    }

    size_t size() const { return size_; }
    AvailableCnt &available_cnt() { return *available_cnt_; }

    size_t take() {
      for (;;) {
        if (available_cnt() == 0) {
          available_cnt().wait(0);
        }
        rp(i, size()) {
          if (available(i)) {
            if (const auto ori = available(i).exchange(false); ori) {
              available_cnt()--;
              return i;
            }
          }
        }
      }
    }

    void release(size_t i) {
      flag(i).store(Status::None, std::memory_order_release);
      available(i).store(true, std::memory_order_release);
      if (auto ori = available_cnt()++; ori == 0) {
        available_cnt().notify_one();
      }
    }

    void wait_for_ready(size_t i) {
      if (flag(i) == Status::Ok) {
        return;
      }
      if (Status::Ok != flag(i).exchange(Status::Waiting)) {
        flag(i).wait(Status::Waiting);
      }
    }

    void wake_ready(size_t i) {
      const auto s = flag(i).exchange(Status::Ok);
      if (s == Status::Waiting) {
        flag(i).notify_one();
      }
    }

    utils::STDCoutGuard &operator[](size_t i) { return std::get<0>(cache_[i]); }
    Available &available(size_t i) { return *std::get<1>(cache_[i]); }
    const Available &available(size_t i) const {
      return *std::get<1>(cache_[i]);
    }
    Flag &flag(size_t i) { return *std::get<2>(cache_[i]); }

  private:
    const size_t size_;
    utils::ConstSizeArray<
        std::tuple<utils::STDCoutGuard, CPUCacheLineAligned<Available>,
                   CPUCacheLineAligned<Flag>>>
        cache_;
    CPUCacheLineAligned<AvailableCnt> available_cnt_{};
  };

  template <typename F>
  size_t Consume(size_t consume_batch_cnt, const size_t consume_batch_size,
                 F &&fn_write) {
    size_t flushed_bytes{};
    auto res = RunWithMutexLock([&] {
      return ConsumeImpl(
          [&](Element &&e) {
            buffer_cache_.wait_for_ready(e);
            auto &data = buffer_cache_[e].buffer();
            if (data.size() != 0) {
              fn_write(data.data(), data.size());
              flushed_bytes += data.size();
            }
            buffer_cache_.release(e);
          },
          consume_batch_cnt, consume_batch_size);
    });
    *flushed_bytes_ += flushed_bytes;
    return res;
  }

  TimePoint Now() const { return system_clock_.now(); }

  size_t FlushedSize() const {
    return flushed_bytes_->load(std::memory_order_relaxed);
  }

  static constexpr size_t kDefaultProducerCap = 512;
  static constexpr size_t kDefaultBuffCacheNum = 4;

  UniqAsyncLog(size_t producer_cap = kDefaultProducerCap,
               size_t buff_cache_num = kDefaultBuffCacheNum,
               SystemClockType &&clock = SystemClockType{},
               SteadyClockType &&steady_clock = SteadyClockType{})
      : spsc_(producer_cap), system_clock_(std::move(clock)),
        steady_clock_(std::move(steady_clock)), buffer_cache_(buff_cache_num) {}

  UniqAsyncLog(SystemClockType &&clock, SteadyClockType &&steady_clock)
      : UniqAsyncLog(kDefaultProducerCap, kDefaultBuffCacheNum,
                     std::move(clock), std::move(steady_clock)) {}

  template <typename CustomerNotifer>
  std::tuple<size_t, utils::STDCoutGuard &, TimePoint>
  PreAdd(CustomerNotifer &&customer_notifier) {
    size_t pos = buffer_cache_.take();
    // size_t last_yield_cnt{};
    // for (;;) {
    //   pos = buffer_cache_.take();
    //   if (pos == -1) {
    //     last_yield_cnt = this_thread_yield(last_yield_cnt);
    //     continue;
    //   }
    //   break;
    // }
    TimePoint now;
    producer_lock().RunWithMutexLock([&] {
      now = Now();
      spsc().Put(Element(pos), [&] { customer_notifier.Wake(); });
    });
    customer_notifier.Wake();
    return {pos, buffer_cache_[pos], now};
  }

  void AfterAdd(size_t pos) { buffer_cache_.wake_ready(pos); }

private:
  template <typename F>
  size_t ConsumeImpl(F &&f, const size_t consume_batch_cnt,
                     const size_t consume_batch_size) {
    size_t cnt = 0;
    rp(i, consume_batch_cnt) {
      rp(j, consume_batch_size) {
        auto res = spsc().TryGet();
        if (!res) {
          spsc().producer_notifier().Wake();
          return cnt;
        } else {
          f(std::move(*res));
          ++cnt;
        }
      }
      spsc().producer_notifier().Wake();
    }
    return cnt;
  }

  SPSC &spsc() const { return spsc_; }
  ProducerLock &producer_lock() const { return producer_lock_; }

private:
  mutable ProducerLock producer_lock_;
  mutable SPSC spsc_;
  mutable BufferCache buffer_cache_;
  SystemClockType system_clock_;
  SteadyClockType steady_clock_;
  // writeable
  CPUCacheLineAligned<std::atomic_uint64_t> flushed_bytes_{};
}; // namespace utils

template <typename Logger, typename LogWriter>
struct AsyncLogFlushWorker final
    : SteadyTaskWithParentNotifer,
      AtomicNotifierWap,
      std::enable_shared_from_this<AsyncLogFlushWorker<Logger, LogWriter>> {
  using IsStop = bool;
  using NeedNextRound = bool;
  using FuncLoggerTask = std::function<NeedNextRound(IsStop)>;
  using Self = AsyncLogFlushWorker;
  using Ptr = std::shared_ptr<Self>;

  struct Worker {
    template <typename F> void Add(F &&f) {
      auto &&[pos, cache, now] = inner().logger()->PreAdd(inner());
      f(cache, now);
      inner().logger()->AfterAdd(pos);
    }

    void Flush() { inner().RunOneRound(); }

    Worker(AsyncLogFlushWorker &inner) : inner_(inner.shared_from_this()) {}

  private:
    AsyncLogFlushWorker &inner() { return *inner_; }
    Ptr inner_;
  };

  Worker IntoWorker() { return Worker(*this); }

  void Run() override {
    notifier().BlockedWait();
    RunOneRound();
  }

  bool IsAvailable() override { return notifier().IsAwake(); }
  void Stop() override { RunOneRound(); }
  bool Wake() {
    if (!notifier().Wake()) {
      return false;
    }
    WakeParent();
    return true;
  }

  static Ptr New(Logger &&logger, LogWriter &&writer) {
    return std::make_shared<AsyncLogFlushWorker>(
        std::forward<Logger>(logger), std::forward<LogWriter>(writer));
  }

  AsyncLogFlushWorker(Logger &&logger, LogWriter &&writer)
      : logger_(std::forward<Logger>(logger)),
        writer_(std::forward<LogWriter>(writer)) {}

  ~AsyncLogFlushWorker() { Stop(); }

private:
  Logger &logger() { return logger_; }
  LogWriter &writer() { return writer_; }

  void RunOneRound() {
    logger()->Consume(
        std::numeric_limits<uint32_t>::max(), 3,
        [this](const char *p, const size_t n) { writer()->Write(p, n); });
  }

private:
  Logger logger_;
  LogWriter writer_;
};

struct AsyncLogger {
  struct FileWriter {
    void Write(const char *p, const size_t n) { std::fwrite(p, 1, n, out); }
    FileWriter *operator->() { return this; }
    FILE *out;
  };
  using Log = UniqAsyncLog<chrono::AsyncSystemClock, chrono::AsyncSteadyClock>;
  using AsyncLogFlushWorker = AsyncLogFlushWorker<Log *, FileWriter>;
  using AsyncLogFlushWorkerPtr = std::shared_ptr<AsyncLogFlushWorker>;
  using Self = AsyncLogger;

  template <typename F> void Add(F &&f) {
    return worker().Add(std::forward<F>(f));
  }
  void Flush() { return worker().Flush(); }

  AsyncLogger(AsyncLogFlushWorker::Worker worker_) : worker_(worker_) {}

  static Self &GlobalStdout() {
    static auto t = GlobalStdoutImpl();
    return t;
  }

private:
  static Self GlobalStdoutImpl() {
    static auto &&async_logger = Log{
        chrono::AsyncSystemClock{chrono::AsyncClock::GlobalInstance().clock()},
        chrono::AsyncSteadyClock{chrono::AsyncClock::GlobalInstance().clock()},
    };
    auto &&steady_task_runner = AsyncSteadyTaskRunner::GlobalInstance();
    auto worker = steady_task_runner.AddTask(
        AsyncLogFlushWorker::New(&async_logger, FileWriter{stdout}));

    return Self(worker);
  }

  AsyncLogFlushWorker::Worker &worker() { return worker_; }

private:
  AsyncLogFlushWorker::Worker worker_;
};

} // namespace utils
