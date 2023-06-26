#pragma once

#include "stl_headers.h"

#define CONCAT_(prefix, suffix) prefix##suffix
#define CONCAT(prefix, suffix) CONCAT_(prefix, suffix)

#define _rep_impl(i, a, b, cmp, op, step)                                      \
  for (int64_t i = (a), CONCAT(__##i##_, __LINE__) = int64_t(b);               \
       i cmp CONCAT(__##i##_, __LINE__); i op(step))
#define rep(i, a, b) _rep_impl(i, a, b, <, +=, 1)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) _rep_impl(i, a, b, <=, +=, 1)
#define _range_step(i, a, b, s) _rep_impl(i, a, b, <, +=, s)
#define _ranged_step(i, a, b, s) _rep_impl(i, a, b, <=, +=, s)
#define rev_repd(i, a, b) _rep_impl(i, a, b, >=, -=, 1)
#define rev_rep(i, a, b) _rep_impl(i, a, b, >, -=, 1)
#define bit_test(num, index) (((num) >> (index)) & 1)

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
#define NO_INLINE __attribute__((__noinline__))

#define CAT(a, b) CONCAT(a, b)
#define FIRST(first, ...) first
#define SECOND(first, second, ...) second
#define MSG_ENDL(ostr)                                                         \
  do {                                                                         \
    ostr << std::endl;                                                         \
  } while (false)
#define COUT_MSG_ENDL() MSG_ENDL(std::cout)
#define MSG_SPLIT(ostr, split, ...)                                            \
  utils::PrintMessage(ostr, split, ##__VA_ARGS__)
#define MSG_NO_SPLIT(ostr, ...) utils::PrintMessage(ostr, "", ##__VA_ARGS__)
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
    COUT_MSGLN_NO_SPLIT("Panic at ", __FILE__, ":", __LINE__);                 \
    utils::PrintlnMessage(std::cerr, " ", ##__VA_ARGS__);                      \
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
    FMSGLN("  {}\n    Which is: {}\n  {}\n    Which is: {}", (#expr1), (r1),   \
           (#expr2), (r2));                                                    \
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
#define DEBUG_MSGLN(...)
#else
#define ASSERT(...) RUNTIME_ASSERT(__VA_ARGS__)
#define ASSERT_EQ(...) RUNTIME_ASSERT_EQ(__VA_ARGS__)
#define ASSERT_GT(...) RUNTIME_ASSERT_GT(__VA_ARGS__)
#define ASSERT_GE(...) RUNTIME_ASSERT_GE(__VA_ARGS__)
#define ASSERT_LT(...) RUNTIME_ASSERT_LT(__VA_ARGS__)
#define ASSERT_LE(...) RUNTIME_ASSERT_LE(__VA_ARGS__)
#define ASSERT_NE(...) RUNTIME_ASSERT_NE(__VA_ARGS__)
#define DEBUG_MSGLN(...) MSGLN(__VA_ARGS__)
#endif

#define SHOW_TIME_COST(...)                                                    \
  utils::TimeCost CONCAT(_time_cost_, __LINE__) { __VA_ARGS__ }

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

#define TO_LOG_MSG_ADD_TIME(str)                                               \
  utils::log_str_add_time(                                                     \
      utils::extract_file_name(std::string_view{__FILE__}), __LINE__, (str))

#define TO_LOG_FMSG_ADD_TIME(fmt_str, ...)                                     \
  TO_LOG_MSG_ADD_TIME(fmt::format(FMT_COMPILE(fmt_str), __VA_ARGS__))

#define TO_LOG_MSG(str)                                                        \
  utils::log_str(utils::extract_file_name(std::string_view{__FILE__}),         \
                 __LINE__, (str))

#define TO_LOG_FMSG(fmt_str, ...)                                              \
  TO_LOG_MSG(fmt::format(FMT_COMPILE(fmt_str), __VA_ARGS__))

// clang-format off
#define TF_GET_1ST_ARG(a, ...) a
#define TF_GET_2ND_ARG(a1, a2, ...) a2
#define TF_GET_3RD_ARG(a1, a2, a3, ...) a3
#define TF_GET_4TH_ARG(a1, a2, a3, a4, ...) a4
#define TF_GET_5TH_ARG(a1, a2, a3, a4, a5, ...) a5
#define TF_GET_6TH_ARG(a1, a2, a3, a4, a5, a6, ...) a6
#define TF_GET_7TH_ARG(a1, a2, a3, a4, a5, a6, a7, ...) a7
#define TF_GET_8TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, ...) a8
#define TF_GET_9TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, ...) a9
#define TF_GET_10TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ...) a10
#define TF_GET_11TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define TF_GET_12TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, ...) a12
#define TF_GET_13TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, ...) a13
#define TF_GET_14TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, ...) a14
#define TF_GET_15TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, ...) a15
#define TF_GET_16TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, ...) a16
#define TF_GET_17TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, ...) a17
#define TF_GET_18TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, ...) a18
#define TF_GET_19TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, ...) a19
#define TF_GET_20TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, ...) a20
#define TF_GET_21TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, ...) a21
#define TF_GET_22TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, ...) a22
#define TF_GET_23TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, ...) a23
#define TF_GET_24TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, ...) a24
#define TF_GET_25TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, ...) a25
#define TF_GET_26TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, ...) a26
#define TF_GET_27TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, ...) a27
#define TF_GET_28TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, ...) a28
#define TF_GET_29TH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, ...) a29
#define LOG_IMPL_CHOSER(...) TF_GET_29TH_ARG(__VA_ARGS__, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_1, LOG_IMPL_0)
// clang-format on

#define LOG_IMPL_0(message)                                                    \
  do {                                                                         \
    auto &&s = TO_LOG_MSG_ADD_TIME((message));                                 \
    utils::STDCout(s);                                                         \
  } while (false)

#define LOG_IMPL_1(fmt_str, ...)                                               \
  do {                                                                         \
    auto &&s = TO_LOG_FMSG_ADD_TIME((fmt_str), __VA_ARGS__);                   \
    utils::STDCout(s);                                                         \
  } while (false)

#define LOG_INFO(...) LOG_IMPL_CHOSER(__VA_ARGS__)(__VA_ARGS__)

#define ASYNC_LOG_IMPL_0(logger, message)                                      \
  do {                                                                         \
    auto &&s = TO_LOG_MSG((message));                                          \
    (*(logger)).Put(std::move(s));                                             \
  } while (false)

#define ASYNC_LOG_IMPL_1(logger, fmt_str, ...)                                 \
  do {                                                                         \
    auto &&s = TO_LOG_FMSG((fmt_str), __VA_ARGS__);                            \
    (*(logger)).Put(std::move(s));                                             \
  } while (false)

#define ASYNC_LOG_INFO(logger, ...)                                            \
  CONCAT(ASYNC_, LOG_IMPL_CHOSER(__VA_ARGS__))((logger), __VA_ARGS__)

#ifdef NDEBUG
#define LOG_DEBUG(...)
#define ASYNC_LOG_DEBUG(...)
#else
#define LOG_DEBUG(...) LOG_INFO(__VA_ARGS__)
#define ASYNC_LOG_DEBUG(...) ASYNC_LOG_INFO(__VA_ARGS__)
#endif

#define FMT_LOG_TIME_POINT "[{:%Y-%m-%d %H:%M:%S}.{:03d}]"
#define FMT_LOG_MSG_DETAIL "[{}:{}][{}][tid={}]"
#define FMT_LOG_MSG FMT_LOG_TIME_POINT FMT_LOG_MSG_DETAIL
