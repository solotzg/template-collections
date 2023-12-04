#include <tests/tests.h>
#include <utils/async-log.hpp>

namespace tests {
using AsyncLogger = utils::UniqAsyncLog<>;

struct AsyncLoggerHelper {
  template <typename F> void Add(F &&f) {
    auto &&[pos, cache, now] = logger_.PreAdd(notifer_);
    f(cache, now);
    logger_.AfterAdd(pos);
  }

  AsyncLogger &logger_;
  utils::AtomicNotifier &notifer_;
};

static void _test_async_log1() {
  std::stringstream ss;
  auto &&fn_write_ss = [&](const char *p, const size_t n) { ss.write(p, n); };
  auto notifer = utils::AtomicNotifier{};

  AsyncLogger logger{2, 2};
  AsyncLoggerHelper logger_helper{logger, notifer};
  {
    HANDLE_LOG_MSG_WITH_TIME_TID(logger_helper.Add, "{}", 123);
    logger.Consume(1, 1, fn_write_ss);
    auto s = ss.str();
    ASSERT_EQ(s[0], '[');
    ASSERT_EQ(s[utils::kLogTimePointSize - 1], ']');
    ASSERT_EQ(s[utils::kLogTimePointSize], '[');
    ASSERT_EQ(s.find(__FILE_NAME__), utils::kLogTimePointSize + 1);
    ASSERT_NE(s.find("123"), s.npos);
    ASSERT_NE(s.find("tid="), s.npos);
    ASSERT_EQ(s[s.size() - 2], ']');
    ASSERT_EQ(s.back(), '\n');
  }
}

static void _test_async_log() { _test_async_log1(); }

} // namespace tests

FUNC_FACTORY_REGISTER("async_log", tests::_test_async_log);
