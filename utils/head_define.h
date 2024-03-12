#pragma once

#include <utils/stl_headers.h>

#define CONCAT_(prefix, suffix) prefix##suffix
#define CONCAT(prefix, suffix) CONCAT_(prefix, suffix)
#define LNAME(name) CONCAT(_##name##_, __LINE__)

#define _rep_impl(i, a, b, cmp, op, step)                                      \
  for (int64_t i = (a), LNAME(i) = int64_t(b); i cmp LNAME(i); i op(step))
#define rep(i, a, b) _rep_impl(i, a, b, <, +=, 1)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) _rep_impl(i, a, b, <=, +=, 1)
#define _range_step(i, a, b, s) _rep_impl(i, a, b, <, +=, s)
#define _ranged_step(i, a, b, s) _rep_impl(i, a, b, <=, +=, s)
#define rev_repd(i, a, b) _rep_impl(i, a, b, >=, -=, 1)
#define rev_rep(i, a, b) _rep_impl(i, a, b, >, -=, 1)
#define bit_test(num, index) (((num) >> (index)) & 1)
#define RESTRICT __restrict
#define OMIT(...)
#define __STR_LINE(x) #x
#define _STR_LINE(x) __STR_LINE(x)
#define STR_LINE _STR_LINE(__LINE__)

typedef long long LL;
typedef unsigned long long ULL;
typedef std::vector<int> VI;
typedef std::vector<LL> VLL;
typedef std::vector<ULL> VULL;
typedef std::pair<int, int> PII;
typedef std::vector<PII> VPII;

typedef __uint128_t U128;
typedef __int128_t I128;
typedef uint64_t U64;
typedef int64_t I64;
typedef uint32_t U32;
typedef int32_t I32;
typedef uint16_t U16;
typedef int16_t I16;
typedef uint8_t U8;
typedef int8_t I8;

#define YIELD_CURRENT_THREAD(n)                                                \
  do {                                                                         \
    rp(_, n) { std::this_thread::yield(); }                                    \
  } while (0)

#define likely(x) (__builtin_expect(!!(x), 1))
#define unlikely(x) (__builtin_expect(!!(x), 0))

#define FLATTEN_INLINE_PURE static __attribute__((flatten, always_inline, pure))
#define ALWAYS_INLINE static __attribute__((always_inline))
#define NO_INLINE __attribute__((__noinline__))
#define FMT_TIMEPOINT_MICROSEC "{:%Y-%m-%d %T}"
#define LOG_TIMEPOINT_SIZE 25
#define ALIGN_TO(addr, align)                                                  \
  reinterpret_cast<decltype(addr)>(U64(addr) / align * align)
#define ALIGN_UP_TO(addr, align)                                               \
  reinterpret_cast<decltype(addr)>((U64(addr) + align - 1) / align * align)

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

#define FMSG(...) fmt::print(__VA_ARGS__)
#define FMSGLN(f, ...) fmt::print((f "\n"), __VA_ARGS__)

#define PANIC(...)                                                             \
  do {                                                                         \
    COUT_MSGLN_NO_SPLIT("Panic at ", __FILE__, ":", __LINE__);                 \
    utils::PrintlnMessage(std::cerr, " ", ##__VA_ARGS__);                      \
    std::terminate();                                                          \
  } while (false)

#define RUNTIME_ASSERT(expr, ...)                                              \
  do {                                                                         \
    auto &&LNAME(r) = (expr);                                                  \
    if (!LNAME(r)) {                                                           \
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
    auto &&LNAME(r1) = (expr1);                                                \
    auto &&LNAME(r2) = (expr2);                                                \
    if (!(LNAME(r1) == LNAME(r2))) {                                           \
      COUT_MSGLN_NO_SPLIT("Not EQ at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, LNAME(r1), expr2, LNAME(r2));                     \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)
#define RUNTIME_ASSERT_NE(expr1, expr2, ...)                                   \
  do {                                                                         \
    auto &&LNAME(r1) = (expr1);                                                \
    auto &&LNAME(r2) = (expr2);                                                \
    if (LNAME(r1) == LNAME(r2)) {                                              \
      COUT_MSGLN_NO_SPLIT("Not NE at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, LNAME(r1), expr2, LNAME(r2));                     \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)
#define RUNTIME_ASSERT_GT(expr1, expr2, ...)                                   \
  do {                                                                         \
    auto &&LNAME(r1) = (expr1);                                                \
    auto &&LNAME(r2) = (expr2);                                                \
    if (!(LNAME(r1) > LNAME(r2))) {                                            \
      COUT_MSGLN_NO_SPLIT("Not GT at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, LNAME(r1), expr2, LNAME(r2));                     \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)
#define RUNTIME_ASSERT_GE(expr1, expr2, ...)                                   \
  do {                                                                         \
    auto &&LNAME(r1) = (expr1);                                                \
    auto &&LNAME(r2) = (expr2);                                                \
    if (!(LNAME(r1) >= LNAME(r2))) {                                           \
      COUT_MSGLN_NO_SPLIT("Not GE at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, LNAME(r1), expr2, LNAME(r2));                     \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)
#define RUNTIME_ASSERT_LT(expr1, expr2, ...)                                   \
  do {                                                                         \
    auto &&LNAME(r1) = (expr1);                                                \
    auto &&LNAME(r2) = (expr2);                                                \
    if (!(LNAME(r1) < LNAME(r2))) {                                            \
      COUT_MSGLN_NO_SPLIT("Not LT at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, LNAME(r1), expr2, LNAME(r2));                     \
      PANIC(__VA_ARGS__);                                                      \
    }                                                                          \
  } while (false)

#define RUNTIME_ASSERT_LE(expr1, expr2, ...)                                   \
  do {                                                                         \
    auto &&LNAME(r1) = (expr1);                                                \
    auto &&LNAME(r2) = (expr2);                                                \
    if (!(LNAME(r1) <= LNAME(r2))) {                                           \
      COUT_MSGLN_NO_SPLIT("Not LE at ", __FILE__, ":", __LINE__);              \
      SHOW_EXPRESSION(expr1, LNAME(r1), expr2, LNAME(r2));                     \
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
  utils::TimeCost LNAME(time_cost) { __VA_ARGS__ }

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

#define EXTRACT_FILE_NAME utils::extract_file_name(std::string_view{__FILE__})

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

#define HANDLE_LOG_MSG_WITH_TIME_TID(logger, fmt_str, ...)                     \
  do {                                                                         \
    auto &&LNAME(buf) =                                                        \
        utils::STDCoutGuard<>::thread_local_instance().buffer();               \
    auto &&LNAME(ts_cache) =                                                   \
        utils::STDCoutGuard<>::thread_local_instance().last_update_time_sec(); \
    LNAME(buf).resize(LOG_TIMEPOINT_SIZE);                                     \
    fmt::format_to(std::back_inserter(LNAME(buf)),                             \
                   "[" __FILE_NAME__ ":" STR_LINE "][" fmt_str "][tid={}]\n",  \
                   __VA_ARGS__, utils::get_str_tid());                         \
    const auto LNAME(tp) = (logger)->Now();                                    \
    utils::SerializeTimepoint(LNAME(buf).data(), LNAME(ts_cache), LNAME(tp));  \
    (logger)->Write(LNAME(buf).data(), LNAME(buf).size(), LNAME(tp));          \
  } while (false)

#define DEF_SYSTEM_CLOCK_NOW utils::SystemClock::now()
#define ASYNC_SYSTEM_CLOCK_NOW                                                 \
  utils::chrono::AsyncClock::GlobalInstance().clock().system_time_point()
#define SYSTEM_CLOCK_NOW DEF_SYSTEM_CLOCK_NOW

#define PRINT_LOG_MSG_WITH_TIME_TID(fmt_str, ...)                              \
  do {                                                                         \
    auto &&LNAME(buf) =                                                        \
        utils::STDCoutGuard<>::thread_local_instance().buffer();               \
    auto &&LNAME(ts_cache) =                                                   \
        utils::STDCoutGuard<>::thread_local_instance().last_update_time_sec(); \
    LNAME(buf).resize(LOG_TIMEPOINT_SIZE);                                     \
    fmt::format_to(std::back_inserter(LNAME(buf)),                             \
                   "[" __FILE_NAME__ ":" STR_LINE "][" fmt_str "][tid={}]\n",  \
                   __VA_ARGS__, utils::get_str_tid());                         \
    utils::RunWithGlobalStdoutLock([&] {                                       \
      utils::SerializeTimepoint(LNAME(buf).data(), LNAME(ts_cache),            \
                                SYSTEM_CLOCK_NOW);                             \
      std::fwrite(LNAME(buf).data(), 1, LNAME(buf).size(), stdout);            \
    });                                                                        \
  } while (false)

#define LOG_IMPL_1(fmt_str, ...)                                               \
  PRINT_LOG_MSG_WITH_TIME_TID(fmt_str, __VA_ARGS__)

#define LOG_IMPL_0(message) LOG_IMPL_1("{}", message)

#define LOG_INFO(...) LOG_IMPL_CHOSER(__VA_ARGS__)(__VA_ARGS__)

#define ASYNC_LOG_IMPL_1(logger, fmt_str, ...)                                 \
  HANDLE_LOG_MSG_WITH_TIME_TID(logger, fmt_str, __VA_ARGS__)

#define ASYNC_LOG_IMPL_0(logger, message)                                      \
  ASYNC_LOG_IMPL_1(logger, "{}", message)

#define ASYNC_LOG_INFO(logger, ...)                                            \
  CONCAT(ASYNC_, LOG_IMPL_CHOSER(__VA_ARGS__))(logger, __VA_ARGS__)

#ifdef NDEBUG
#define LOG_DEBUG(...)
#define ASYNC_LOG_DEBUG(...)
#else
#define LOG_DEBUG(...) LOG_INFO(__VA_ARGS__)
#define ASYNC_LOG_DEBUG(...) ASYNC_LOG_INFO(__VA_ARGS__)
#endif

#ifdef NDEBUG
#define DEBUG_SCOPE(...)
#else
#define DEBUG_SCOPE(...) [&]() { __VA_ARGS__; }()
#endif
#define DEBUG_FMSGLN(...) DEBUG_SCOPE({ FMSGLN(__VA_ARGS__); })
