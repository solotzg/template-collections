#pragma once

#ifndef USE_STD_COROUTINE

// libc++'s <coroutine> header only provides its declarations for C++20 and
// above, so we need to fall back to <experimental/coroutine> when building with
// C++17.
#if __has_include(<coroutine>) && (!defined(_LIBCPP_VERSION) || __cplusplus > 201703L)
#define USE_STD_COROUTINE 1
#else
#define USE_STD_COROUTINE 0
#endif

#endif

#if USE_STD_COROUTINE
#include <coroutine>
namespace std_coro = std;
#else
#include <experimental/coroutine>
namespace std_coro = std::experimental;
#endif