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
  };

  using SPSC = SPSCQueueWithNotifer<SPSCQueue<Element>>;
  using ProducerLock = MutexLockWrap;
  using Buffer = std::string;

  size_t Consume(size_t consume_batch_cnt, const size_t consume_batch_size,
                 std::ostream &ostr = std::cout) {
    std::array<char, utils::LogTimePointSize> buff;
    auto res = RunWithMutexLock([&] {
      return ConsumeImpl(
          [&](Element &&msg) {
            auto &&now = msg.time;
            auto &&millisec =
                duration_cast<Milliseconds>(now.time_since_epoch()).count() %
                1000;
            fmt::format_to_n(buff.data(), buff.size(),
                             FMT_COMPILE(FMT_LOG_TIME_POINT), now, millisec);
            for (std::string_view s{buff.data(), buff.size()}; !s.empty();) {
              if (full()) {
                flush(ostr);
              }
              auto cp_size = std::min(s.size(), buff_remain_size());
              put_buffer(s.substr(0, cp_size));
              s.remove_prefix(cp_size);
            }
            for (std::string_view s = msg.msg; !s.empty();) {
              if (full()) {
                flush(ostr);
              }
              auto cp_size = std::min(s.size(), buff_remain_size());
              put_buffer(s.substr(0, cp_size));
              s.remove_prefix(cp_size);
            }
            {
              if (full()) {
                flush(ostr);
              }
              put_buffer('\n');
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

  void Flush(std::ostream &ostr) {
    return RunWithMutexLock([&] { flush(ostr); });
  }

  size_t Size() const {
    return RunWithMutexLock([&] { return size_; });
  }
  size_t MaxBuffSize() const { return buffer_.size(); }
  size_t RemainBuffeSize() { return MaxBuffSize() - Size(); }
  size_t FlushedSize() const {
    return flushed_bytes_.load(std::memory_order_relaxed);
  }

  static constexpr size_t default_producer_cap = 512;
  static constexpr size_t default_max_buff_size = 1024 * 4;

  UniqAsyncLog(size_t producer_cap = default_producer_cap,
               size_t max_buff_size = default_max_buff_size,
               SystemClockType &&clock = SystemClockType{},
               SteadyClockType &&steady_clock = SteadyClockType{})
      : spsc_(producer_cap), system_clock_(std::move(clock)),
        steady_clock_(std::move(steady_clock)) {
    ASSERT_GT(max_buff_size, 0);
    buffer_.resize(max_buff_size);
  }

  UniqAsyncLog(SystemClockType &&clock, SteadyClockType &&steady_clock)
      : UniqAsyncLog(default_producer_cap, default_max_buff_size,
                     std::move(clock), std::move(steady_clock)) {}

  template <typename CustomerNotifer>
  void Add(Message &&msg, CustomerNotifer &&customer_notifier) const {
    producer_lock().RunWithMutexLock([&] {
      auto &&now = Now();
      bool res = spsc().Put(Element(now, std::move(msg)), MaxDuration,
                            [&] { customer_notifier.Wake(); });
      RUNTIME_ASSERT(res);
      customer_notifier.Wake();
    });
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

  void flush(std::ostream &ostr) {
    if (empty())
      return;
    ostr << std::string_view{begin(), size()};
    ostr.flush();
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
  char *begin() { return buffer_.data(); }
  char *end() { return buffer_.data() + size(); }
  bool full() const { return size() == MaxBuffSize(); }
  void reset() { size_ = 0; }

private:
  mutable ProducerLock producer_lock_;
  mutable SPSC spsc_;
  mutable Buffer buffer_;
  size_t size_{};
  std::atomic<typename SteadyClockType::time_point> last_flush_time_{};
  std::atomic_uint64_t flushed_bytes_{};
  SystemClockType system_clock_;
  SteadyClockType steady_clock_;
};

template <typename Log>
struct AsyncLogFlushWorker final
    : SteadyTaskWithParentNotifer,
      AtomicNotifierWap,
      std::enable_shared_from_this<AsyncLogFlushWorker<Log>> {
  using IsStop = bool;
  using NeedNextRound = bool;
  using FuncLoggerTask = std::function<NeedNextRound(IsStop)>;
  using Self = AsyncLogFlushWorker;
  using Ptr = std::shared_ptr<Self>;

  struct Worker {
    template <typename... Args> void RegisterSteadyFlushLog(Args &&...args) {
      inner_->RegisterSteadyFlushLog(std::forward<Args>(args)...);
    }
    void Put(typename Log::Message &&msg) {
      (*inner_).logger()->Add(std::move(msg), (*inner_));
    }
    std::shared_ptr<AsyncLogFlushWorker> inner_;
  };

  Worker IntoWorker() { return Worker{this->shared_from_this()}; }

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

  static Ptr New(Log *logger, std::ostream &ostr) {
    return std::make_shared<AsyncLogFlushWorker>(logger, ostr);
  }

  AsyncLogFlushWorker(Log *logger, std::ostream &ostr)
      : logger_(logger), ostr_(ostr) {}

  ~AsyncLogFlushWorker() { Stop(); }

private:
  Log *logger() { return logger_; }

  template <typename Timer>
  void RegisterSteadyFlushLog(Timer &timer, Milliseconds duration,
                              Milliseconds flush_dur) {
    RegisterFlushLogTimer(this->shared_from_this(), false, timer, duration,
                          flush_dur);
  }

  void TriggerFlush() { need_flush_log_ = true; }

  void TryTriggerFlush(bool stop, Milliseconds flush_timeout) {
    if (logger_->NeedFlush(flush_timeout)) {
      TriggerFlush();
      Wake();
    }
  }

  void RunOneRound() {
    logger_->Consume(std::numeric_limits<uint32_t>::max(), 32, ostr_);
    if (need_flush_log_.exchange(false)) {
      logger_->Flush(ostr_);
    }
  }

private:
  Log *logger_{};
  std::ostream &ostr_;
  std::atomic_bool need_flush_log_;
};

struct AsyncLogger {
  using Log = UniqAsyncLog<chrono::AsyncSystemClock, chrono::AsyncSteadyClock>;
  using AsyncLogFlushWorker = AsyncLogFlushWorker<Log>;
  using AsyncLogFlushWorkerPtr = std::shared_ptr<AsyncLogFlushWorker>;
  using Self = AsyncLogger;

  void Put(Log::Message &&msg) { worker().Put(std::move(msg)); }

  AsyncLogger(AsyncLogFlushWorker::Worker worker_) : worker_(worker_) {}

  static Self &GlobalSTDOUT() {
    static auto t = GlobalAsyncLogger(std::cout);
    return t;
  }

private:
  static Self GlobalAsyncLogger(std::ostream &ostr) {
    static auto &&async_logger = Log{
        chrono::AsyncSystemClock{chrono::AsyncClock::GlobalInstance().clock()},
        chrono::AsyncSteadyClock{chrono::AsyncClock::GlobalInstance().clock()},
    };
    auto &&timer = WheelTimerRunner::GlobalInstance();
    auto &&steady_task_runner = AsyncSteadyTaskRunner::GlobalInstance();

    auto worker = steady_task_runner.AddTask(
        AsyncLogFlushWorker::New(&async_logger, ostr));
    worker.RegisterSteadyFlushLog(timer, Milliseconds{32}, Milliseconds{64});

    return Self(worker);
  }

  AsyncLogFlushWorker::Worker &worker() { return worker_; }

private:
  AsyncLogFlushWorker::Worker worker_;
};

} // namespace utils

#ifndef NDEBUG
namespace tests {

struct AsyncLogNotifer : utils::AtomicNotifierWap {
  bool IsAwake() const { return notifier().IsAwake(); }
  bool Wake() const {
    if (!notifier().Wake()) {
      return false;
    }
    upper_notifier_.Wake();
    return true;
  }
  void BlockedWait() { return notifier().BlockedWait(); }
  AsyncLogNotifer(utils::AsyncNotifier &upper_notifier)
      : upper_notifier_(upper_notifier) {}

  utils::AsyncNotifier &upper_notifier_;
};

static void _test_async_log1() {
  std::stringstream ss;
  auto global_notifer = utils::Notifier{};
  auto async_log_notifier = AsyncLogNotifer{global_notifer};
  utils::UniqAsyncLog<> logger{2, 4 + utils::LogTimePointSize};
  {
    logger.Add("1", async_log_notifier);
    logger.Consume(1, 1, ss);
    ASSERT_EQ(logger.Size(), 2 + utils::LogTimePointSize);
    ASSERT_EQ(logger.RemainBuffeSize(), 2);
    logger.Flush(ss);
    auto s = ss.str();
    ASSERT_EQ(std::string_view(s.data() + utils::LogTimePointSize, 1), "1");
  }
  {
    logger.Add("2", async_log_notifier);
    logger.Consume(1, 1, ss);
    ASSERT_EQ(logger.Size(), 2 + utils::LogTimePointSize);
    ASSERT_EQ(logger.RemainBuffeSize(), 2);
  }
  {
    logger.Add("3", async_log_notifier);
    logger.Consume(1, 1, ss);
    ASSERT_EQ(logger.Size(), 25);
    ASSERT_EQ(logger.RemainBuffeSize(), 4);
    logger.Flush(ss);
    ASSERT_EQ((utils::LogTimePointSize + 2) * 3, ss.str().size());
  }
  { ss.str(""); }
  {
    ASSERT(logger.NeedFlush(utils::Nanoseconds{1}));
    ASSERT(!logger.NeedFlush(utils::Seconds{1024}));
  }
  {
    async_log_notifier.Wake();
    async_log_notifier.BlockedWait();
    ASSERT(!async_log_notifier.IsAwake());
    logger.Add("x", async_log_notifier);
    async_log_notifier.BlockedWait();
    logger.Consume(1, 1, ss);
    logger.Flush(ss);
  }
  {
    ss.clear();
    ss.str("");
    auto ori_size = logger.FlushedSize();
    std::list<std::thread> async_runners;
    std::atomic_uint64_t join_cnt = 0;
    const size_t loop_cnt = 256;
    const size_t thread_cnt = 4;
    utils::Waiter waiter;
    rp(i, thread_cnt) async_runners.emplace_back([&, i, loop_cnt] {
      waiter.Wait();
      rp(j, loop_cnt) {
        auto &&s = fmt::format(FMT_COMPILE("{:05d}:{:05d}"), i, j);
        logger.Add(std::move(s), async_log_notifier);
      }
      join_cnt++;
      global_notifer.Wake();
    });
    waiter.WakeAll();
    size_t expect_size =
        async_runners.size() * (11 + 1 + utils::LogTimePointSize) * (loop_cnt);
    for (;;) {
      bool stop = join_cnt == async_runners.size();
      if (const auto status =
              global_notifer.BlockedWaitFor(utils::Milliseconds{5});
          status == utils::Notifier::Status::Normal) {
        if (async_log_notifier.IsAwake()) {
          async_log_notifier.BlockedWait();
          logger.Consume(std::numeric_limits<uint32_t>::max(), 2, ss);
        }
      }
      if (logger.NeedFlush(utils::Milliseconds{50})) {
        logger.Flush(ss);
      }
      if (stop)
        break;
    }
    for (auto &&t : async_runners)
      t.join();
    logger.Flush(ss);
    auto flushed_size = logger.FlushedSize() - ori_size;
    auto res = ss.str();
    ASSERT_EQ(flushed_size, res.size());
    ASSERT_EQ(flushed_size, expect_size);
  }
}

static void _test_async_log2() {
  std::stringstream ss;
  utils::UniqAsyncLog<> logger{2, 4096};
  {
    using TaskPoolWorker = utils::TaskPoolWorker<>;
    ss.clear();
    ss.str("");

    auto waiter_task_pool = utils::Waiter::New();

    auto task_pool = std::make_shared<TaskPoolWorker>();

    auto log_flush =
        utils::AsyncLogFlushWorker<utils::UniqAsyncLog<>>::New(&logger, ss);

    Timer timer;
    timer.AsyncRun();
    utils::AsyncSteadyTaskRunner steady_task_runner;
    steady_task_runner.AsyncRun();

    std::list<std::thread> async_runners;
    const size_t ori_size = logger.FlushedSize();
    std::atomic_uint64_t join_cnt = 0;
    utils::Waiter waiter;

    auto task_pool_worker = steady_task_runner.AddTask(task_pool);
    auto log_flush_worker = steady_task_runner.AddTask(log_flush);
    task_pool_worker.Put(
        [waiter_task_pool](bool stop) -> bool {
          ASSERT(!stop);
          waiter_task_pool->WakeAll();
          return true;
        },
        {});
    waiter_task_pool->Wait();

    constexpr size_t loop_cnt = 256;
    constexpr size_t async_runner_cnt = 4;

    rp(i, async_runner_cnt) async_runners.emplace_back([&, i, loop_cnt] {
      waiter.Wait();
      rp(j, loop_cnt) {
        auto &&s = fmt::format(FMT_COMPILE("{:05d}:{:05d}"), i, j);
        logger.Add(std::move(s), *log_flush);
      }
      join_cnt++;
    });

    log_flush_worker.RegisterSteadyFlushLog(timer, utils::Milliseconds{1},
                                            utils::Milliseconds{4});

    waiter.WakeAll();
    for (auto &&t : async_runners)
      t.join();
    {
      timer.StopAndWait();
      steady_task_runner.StopAndWait();
    }
    auto flushed_size = logger.FlushedSize() - ori_size;
    auto res = ss.str();
    ASSERT_EQ(flushed_size, res.size());
    size_t expect_size =
        async_runners.size() * (11 + 1 + utils::LogTimePointSize) * (loop_cnt);
    ASSERT_EQ(flushed_size, expect_size);
  }
}

static void _test_async_log() {
  _test_async_log1();
  _test_async_log2();
}

} // namespace tests
#endif