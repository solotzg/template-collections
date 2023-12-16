#include <tests/tests.h>
#include <utils/async-log.hpp>

namespace tests {
using AsyncLogger = utils::UniqAsyncLog;

struct AsyncLoggerHelper {
  using TimePoint = utils::SysMilliseconds;

  TimePoint Now() {
    return std::chrono::time_point_cast<utils::Milliseconds>(
        DEF_SYSTEM_CLOCK_NOW);
  }

  void Write(char *src, size_t n, TimePoint time_point) {
    logger_.Write(src, n, time_point,
                  [&](const char *p, size_t n) { writer_(p, n); });
  }

  void Flush() {
    logger_.Flush([&](const char *p, size_t n) { writer_(p, n); });
  }

  AsyncLogger &logger_;
  std::function<void(const char *, size_t)> writer_;
};

static void _test_async_log1() {
  std::stringstream ss;

  AsyncLogger logger;
  AsyncLoggerHelper logger_helper{
      logger, [&](const char *p, const size_t n) { ss.write(p, n); }};
  {
    HANDLE_LOG_MSG_WITH_TIME_TID(&logger_helper, "{}", 123);
    logger_helper.Flush();
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
