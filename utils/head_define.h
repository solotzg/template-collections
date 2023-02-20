#pragma once

#include <cstdint>
#include <vector>

#define CONCAT_(prefix, suffix) prefix##suffix
#define CONCAT(prefix, suffix) CONCAT_(prefix, suffix)

#define _rep_impl(i, a, b, cmp, op, step)                                      \
  for (int64_t i = (a), CONCAT(__##i##_, __LINE__) = int64_t(b);               \
       i cmp CONCAT(__##i##_, __LINE__); i op step)
#define rep(i, a, b) _rep_impl(i, a, b, <, +=, 1)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) _rep_impl(i, a, b, <=, +=, 1)
#define rev_repd(i, a, b) _rep_impl(i, a, b, >=, -=, 1)
#define rev_rep(i, a, b) _rep_impl(i, a, b, >, -=, 1)
#define bit(num, index) (((num) >> (index)) & 1)

typedef long long LL;
typedef unsigned long long ULL;
typedef std::vector<int> VI;
typedef std::vector<LL> VLL;
typedef std::vector<ULL> VULL;
typedef std::pair<int, int> PII;
typedef std::vector<PII> VPII;

#ifdef __GNUC__

typedef unsigned __int128 UINT128;
typedef __int128 INT128;

#endif

#define likely(x) (__builtin_expect(!!(x), 1))
#define unlikely(x) (__builtin_expect(!!(x), 0))

#define FLATTEN_INLINE_PURE static __attribute__((flatten, always_inline, pure))
#define ALWAYS_INLINE static __attribute__((always_inline))

#define CAT(a, b) CONCAT(a, b)
#define FIRST(first, ...) first
#define SECOND(first, second, ...) second
#define MSG_ENDL(ostr)                                                         \
  do {                                                                         \
    ostr << std::endl;                                                         \
  } while (false)
#define COUT_MSG_ENDL() MSG_ENDL(std::cout)
#define MSG_SPLIT(ostr, split, ...) PrintMessage(ostr, split, ##__VA_ARGS__)
#define MSG_NO_SPLIT(ostr, ...) PrintMessage(ostr, "", ##__VA_ARGS__)
#define COUT_MSG_SPLIT(...) MSG_SPLIT(std::cout, ##__VA_ARGS__)
#define COUT_MSGLN_SPLIT(s, ...)                                               \
  do {                                                                         \
    COUT_MSG_SPLIT(s, ##__VA_ARGS__);                                          \
    COUT_MSG_ENDL();                                                           \
  } while (false)
#define COUT_MSG_NO_SPLIT(...) MSG_NO_SPLIT(std::cout, ##__VA_ARGS__)
#define COUT_MSGLN_NO_SPLIT(...)                                               \
  do {                                                                         \
    COUT_MSG_NO_SPLIT(__VA_ARGS__);                                            \
    COUT_MSG_ENDL();                                                           \
  } while (false)
#define MSG(...) COUT_MSG_SPLIT(" ", ##__VA_ARGS__)
#define MSGLN(...)                                                             \
  do {                                                                         \
    MSG(__VA_ARGS__);                                                          \
    COUT_MSG_ENDL();                                                           \
  } while (false)
#define FMSG(...) MSG(fmt::format(__VA_ARGS__))
#define FMSGLN(...)                                                            \
  do {                                                                         \
    FMSG(__VA_ARGS__);                                                         \
    COUT_MSG_ENDL();                                                           \
  } while (false)

#define PANIC(...)                                                             \
  do {                                                                         \
    PrintlnMessage(std::cerr, " ", ##__VA_ARGS__);                             \
    std::terminate();                                                          \
  } while (false)

#define RUNTIME_ASSERT(expr, ...)                                              \
  do {                                                                         \
    auto &&r = (expr);                                                         \
    if (!r) {                                                                  \
      COUT_MSGLN_NO_SPLIT("Assert fails at ", __FILE__, ":", __LINE__);        \
      COUT_MSGLN_NO_SPLIT("  ", #expr);                                        \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)
#define SHOW_EXPRESSION(expr1, r1, expr2, r2)                                  \
  do {                                                                         \
    COUT_MSGLN_NO_SPLIT("  ", #expr1);                                         \
    COUT_MSGLN_NO_SPLIT("    Which is: ", r1);                                 \
    COUT_MSGLN_NO_SPLIT("  ", #expr2);                                         \
    COUT_MSGLN_NO_SPLIT("    Which is: ", r2);                                 \
  } while (false)
#define RUNTIME_ASSERT_EQ(expr1, expr2, ...)                                   \
  do {                                                                         \
    auto &&r1 = (expr1);                                                       \
    auto &&r2 = (expr2);                                                       \
    if (!(r1 == r2)) {                                                         \
      COUT_MSGLN_NO_SPLIT("Not EQ at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, r1, expr2, r2);                                   \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)
#define RUNTIME_ASSERT_NE(expr1, expr2, ...)                                   \
  do {                                                                         \
    auto &&r1 = (expr1);                                                       \
    auto &&r2 = (expr2);                                                       \
    if (r1 == r2) {                                                            \
      COUT_MSGLN_NO_SPLIT("Not NE at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, r1, expr2, r2);                                   \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)
#define RUNTIME_ASSERT_GT(expr1, expr2, ...)                                   \
  do {                                                                         \
    auto &&r1 = (expr1);                                                       \
    auto &&r2 = (expr2);                                                       \
    if (!(r1 > r2)) {                                                          \
      COUT_MSGLN_NO_SPLIT("Not GT at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, r1, expr2, r2);                                   \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)
#define RUNTIME_ASSERT_GE(expr1, expr2, ...)                                   \
  do {                                                                         \
    auto &&r1 = (expr1);                                                       \
    auto &&r2 = (expr2);                                                       \
    if (!(r1 >= r2)) {                                                         \
      COUT_MSGLN_NO_SPLIT("Not GE at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, r1, expr2, r2);                                   \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)
#define RUNTIME_ASSERT_LT(expr1, expr2, ...)                                   \
  do {                                                                         \
    auto &&r1 = (expr1);                                                       \
    auto &&r2 = (expr2);                                                       \
    if (!(r1 < r2)) {                                                          \
      COUT_MSGLN_NO_SPLIT("Not LT at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, r1, expr2, r2);                                   \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)

#define RUNTIME_ASSERT_LE(expr1, expr2, ...)                                   \
  do {                                                                         \
    auto &&r1 = (expr1);                                                       \
    auto &&r2 = (expr2);                                                       \
    if (!(r1 <= r2)) {                                                         \
      COUT_MSGLN_NO_SPLIT("Not LE at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, r1, expr2, r2);                                   \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)

#ifdef NDEBUG
#define ASSERT(...)
#define ASSERT_EQ(...)
#define ASSERT_GT(...)
#define ASSERT_GE(...)
#define ASSERT_LT(...)
#define ASSERT_LE(...)
#define ASSERT_NE(...)
#else
#define ASSERT(...) RUNTIME_ASSERT(__VA_ARGS__)
#define ASSERT_EQ(...) RUNTIME_ASSERT_EQ(__VA_ARGS__)
#define ASSERT_GT(...) RUNTIME_ASSERT_GT(__VA_ARGS__)
#define ASSERT_GE(...) RUNTIME_ASSERT_GE(__VA_ARGS__)
#define ASSERT_LT(...) RUNTIME_ASSERT_LT(__VA_ARGS__)
#define ASSERT_LE(...) RUNTIME_ASSERT_LE(__VA_ARGS__)
#define ASSERT_NE(...) RUNTIME_ASSERT_NE(__VA_ARGS__)
#endif

#define SHOW_TIME_COST                                                         \
  TimeCost CONCAT(time_cost, __LINE__) {}

#define FMT_APPEND(out, ...)                                                   \
  do {                                                                         \
    fmt::format_to(std::back_inserter(out), __VA_ARGS__);                      \
  } while (false)
#define FMT_IF_APPEND(out, expr, ...)                                          \
  if (expr) {                                                                  \
    FMT_APPEND(out, __VA_ARGS__);                                              \
  }
#define SCOPE_EXIT_FMT_SURROUND(out, str_begin, str_end)                       \
  fmt::format_to(std::back_inserter(out), "{}", str_begin);                    \
  SCOPE_EXIT({ fmt::format_to(std::back_inserter(out), "{}", str_end); })
