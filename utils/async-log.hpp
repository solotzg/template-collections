#pragma once

#include "utils/async.hpp"

namespace utils {

template <typename SystemClockType = SystemClock,
          typename SteadyClockType = SteadyClock>
struct UniqAsyncLog : MutexLockWrap {
  using Message = std::string;
  using TimePoint = typename SystemClockType::time_point;
  using Self = UniqAsyncLog;

  struct Element : noncopyable {
    TimePoint time;
    Message msg;

    Element(TimePoint time, Message &&msg) : time(time), msg(std::move(msg)) {}
    Element(Element &&src) : time(src.time), msg(std::move(src.msg)) {}
    Element &operator=(Element &&src) {
      if (this == &src)
        return *this;
      time = src.time;
      msg = std::move(src.msg);
      return *this;
    }

    operator std::string_view() const { return msg; }
  };

  using SPSC = SPSCQueueWithNotifer<SPSCQueue<Element>>;
  using ProducerLock = MutexLockWrap;
  template <size_t align> struct Buffer {
    char *data_{};
    size_t size_;
    Buffer(size_t n) : size_(n) {
      ASSERT_GT(n, 0);
      data_ = (char *)(std::aligned_alloc(align, n));
    }
    size_t size() const { return size_; }
    char *start() {
      __builtin_assume(U64(data_) % align == 0);
      return data_;
    };
    Buffer(const Buffer &) = delete;
    ~Buffer() { std::free(data_); }
  };

  template <typename F>
  size_t Consume(size_t consume_batch_cnt, const size_t consume_batch_size,
                 F &&fn_write) {
    auto res = RunWithMutexLock([&] {
      return ConsumeImpl(
          [&](Element &&msg) {
            std::string_view buff(
                utils::ThreadLocalSerializeTimepoint(msg.time),
                utils::kLogTimePointSize);
            for (std::string_view s = buff; !s.empty();) {
              if (full()) {
                flush(fn_write);
              }
              auto cp_size = std::min(s.size(), buff_remain_size());
              put_buffer(s.substr(0, cp_size));
              s.remove_prefix(cp_size);
            }
            for (std::string_view s = msg; !s.empty();) {
              if (full()) {
                flush(fn_write);
              }
              auto cp_size = std::min(s.size(), buff_remain_size());
              put_buffer(s.substr(0, cp_size));
              s.remove_prefix(cp_size);
            }
            {
              if (full()) {
                flush(fn_write);
              }
              put_buffer(utils::CRLF);
            }
          },
          consume_batch_cnt, consume_batch_size);
    });
    return res;
  }

  TimePoint Now() const { return system_clock_.now(); }

  bool NeedFlush(Nanoseconds dur) const {
    return LastFlushTime() + dur <= steady_clock_.now();
  }

  typename SteadyClockType::time_point LastFlushTime() const {
    return last_flush_time_.load(std::memory_order_relaxed);
  }

  template <typename F> void Flush(F &&f) {
    return RunWithMutexLock([&] { flush(std::forward<F>(f)); });
  }

  size_t Size() const {
    return RunWithMutexLock([&] { return size_; });
  }
  size_t MaxBuffSize() const { return buffer_.size(); }
  size_t RemainBuffeSize() { return MaxBuffSize() - Size(); }
  size_t FlushedSize() const {
    return flushed_bytes_.load(std::memory_order_relaxed);
  }

  static constexpr size_t kDefaultProducerCap = 512;
  static constexpr size_t kDefaultMaxBuffSize = 1024 * 4;
  static constexpr size_t kDefaultBuffAlign = 64;

  UniqAsyncLog(size_t producer_cap = kDefaultProducerCap,
               size_t max_buff_size = kDefaultMaxBuffSize,
               SystemClockType &&clock = SystemClockType{},
               SteadyClockType &&steady_clock = SteadyClockType{})
      : spsc_(producer_cap), system_clock_(std::move(clock)),
        steady_clock_(std::move(steady_clock)), buffer_(max_buff_size) {}

  UniqAsyncLog(SystemClockType &&clock, SteadyClockType &&steady_clock)
      : UniqAsyncLog(kDefaultProducerCap, kDefaultMaxBuffSize, std::move(clock),
                     std::move(steady_clock)) {}

  template <typename CustomerNotifer>
  void Add(Message &&msg, CustomerNotifer &&customer_notifier) const {
    producer_lock().RunWithMutexLock([&] {
      auto &&now = Now();
      bool res = spsc().Put(Element(now, std::move(msg)), MaxDuration,
                            [&] { customer_notifier.Wake(); });
      RUNTIME_ASSERT(res);
    });
    customer_notifier.Wake();
  }

private:
  template <typename F>
  size_t ConsumeImpl(F &&f, const size_t consume_batch_cnt,
                     const size_t consume_batch_size) {
    size_t cnt = 0;
    rp(i, consume_batch_cnt) {
      rp(j, consume_batch_size) {
        auto res = spsc().TryGet();
        if (!res) {
          spsc().WakeProducer();
          return cnt;
        } else {
          f(std::move(*res));
          ++cnt;
        }
      }
      spsc().WakeProducer();
    }
    return cnt;
  }

  SPSC &spsc() const { return spsc_; }
  ProducerLock &producer_lock() const { return producer_lock_; }

  template <typename F> void flush(F &&fn_write) {
    if (empty())
      return;
    fn_write(begin(), size());
    flushed_bytes_ += size();
    reset();
    last_flush_time_ = steady_clock_.now();
  }

  void put_buffer(std::string_view s) {
    std::memcpy(end(), s.data(), s.size());
    size_ += s.size();
  }
  void put_buffer(char c) {
    end()[0] = c;
    size_ += 1;
  }
  size_t size() const { return size_; }
  bool empty() const { return size() == 0; }
  size_t buff_remain_size() { return MaxBuffSize() - size(); }
  char *begin() { return buffer_.start(); }
  char *end() { return buffer_.start() + size(); }
  bool full() const { return size() == MaxBuffSize(); }
  void reset() { size_ = 0; }

private:
  mutable ProducerLock producer_lock_;
  mutable SPSC spsc_;
  mutable Buffer<kDefaultBuffAlign> buffer_;
  size_t size_{};
  std::atomic<typename SteadyClockType::time_point> last_flush_time_{};
  std::atomic_uint64_t flushed_bytes_{};
  SystemClockType system_clock_;
  SteadyClockType steady_clock_;
};

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
    template <typename... Args> void RegisterSteadyFlushLog(Args &&...args) {
      inner().RegisterSteadyFlushLog(std::forward<Args>(args)...);
    }
    template <typename LoggerMessageType> void Put(LoggerMessageType &&msg) {
      inner().logger()->Add(std::forward<LoggerMessageType>(msg), inner());
    }
    void Flush() {
      inner().logger()->Flush([this](const char *p, const size_t n) {
        inner().writer()->Write(p, n);
      });
    }

    Worker(AsyncLogFlushWorker &inner) : inner_(inner.shared_from_this()) {}

  private:
    AsyncLogFlushWorker &inner() { return *inner_; }
    Ptr inner_;
  };

  Worker IntoWorker() { return Worker(*this); }

  template <typename Timer>
  static void RegisterFlushLogTimer(Ptr worker, bool stop, Timer &timer,
                                    const Milliseconds delay,
                                    const Milliseconds flush_timeout) {
    timer.Schedule(
        [&timer, delay, flush_timeout, worker](bool stop) {
          RunFlushLog(worker, stop, timer, delay, flush_timeout);
        },
        delay);
  }

  template <typename Timer>
  static void RunFlushLog(Ptr worker, bool stop, Timer &timer,
                          const Milliseconds delay,
                          const Milliseconds flush_timeout) {
    worker->TryTriggerFlush(stop, flush_timeout);
    if (stop)
      return;
    RegisterFlushLogTimer(std::move(worker), stop, timer, delay, flush_timeout);
  }

  void Run() override {
    notifier().BlockedWait();
    RunOneRound();
  }
  bool IsAvailable() override { return notifier().IsAwake(); }
  void Stop() override {
    TriggerFlush();
    RunOneRound();
  }
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

  template <typename Timer>
  void RegisterSteadyFlushLog(Timer &timer, Milliseconds duration,
                              Milliseconds flush_dur) {
    RegisterFlushLogTimer(this->shared_from_this(), false, timer, duration,
                          flush_dur);
  }

  void TriggerFlush() { need_flush_log_ = true; }

  void TryTriggerFlush(bool stop, Milliseconds flush_timeout) {
    if (logger()->NeedFlush(flush_timeout)) {
      TriggerFlush();
      Wake();
    }
  }

  void RunOneRound() {
    logger()->Consume(
        std::numeric_limits<uint32_t>::max(), 32,
        [this](const char *p, const size_t n) { writer()->Write(p, n); });
    if (need_flush_log_.exchange(false)) {
      logger()->Flush(
          [this](const char *p, const size_t n) { writer()->Write(p, n); });
    }
  }

private:
  Logger logger_;
  LogWriter writer_;
  std::atomic_bool need_flush_log_;
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

  void Put(Log::Message &&msg) { worker().Put(std::move(msg)); }
  void Flush() { worker().Flush(); }

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

    auto &&timer = WheelTimerRunner::GlobalInstance();
    auto &&steady_task_runner = AsyncSteadyTaskRunner::GlobalInstance();

    auto worker = steady_task_runner.AddTask(
        AsyncLogFlushWorker::New(&async_logger, FileWriter{stdout}));
    worker.RegisterSteadyFlushLog(timer, Milliseconds{32}, Milliseconds{64});

    return Self(worker);
  }

  AsyncLogFlushWorker::Worker &worker() { return worker_; }

private:
  AsyncLogFlushWorker::Worker worker_;
};

} // namespace utils
