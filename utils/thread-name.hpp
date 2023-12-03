#pragma once

#include <utils/thread-name.h>

#if defined(__linux__)
#define INNER_DETAIL_HAS_PRCTL_PR_SET_NAME 1
#else
#define INNER_DETAIL_HAS_PRCTL_PR_SET_NAME 0
#endif

#if INNER_DETAIL_HAS_PRCTL_PR_SET_NAME
#include <sys/prctl.h>
#endif

#if defined(__GLIBC__) && !defined(__APPLE__)

#if __GLIBC_PREREQ(2, 12)
// has pthread_setname_np(pthread_t, const char*) (2 params)
#define INNER_HAS_PTHREAD_SETNAME_NP_THREAD_NAME 1
#else
#define INNER_HAS_PTHREAD_SETNAME_NP_THREAD_NAME 0
#endif

#else
#define INNER_HAS_PTHREAD_SETNAME_NP_THREAD_NAME 0
#endif

#if defined(__APPLE__)
#if defined(__MAC_OS_X_VERSION_MIN_REQUIRED) &&                                \
    __MAC_OS_X_VERSION_MIN_REQUIRED >= 1060
// macOS 10.6+ has pthread_setname_np(const char*) (1 param)
#define INNER_HAS_PTHREAD_SETNAME_NP_NAME 1
#elif defined(__IPHONE_OS_VERSION_MIN_REQUIRED) &&                             \
    __IPHONE_OS_VERSION_MIN_REQUIRED >= 30200
// iOS 3.2+ has pthread_setname_np(const char*) (1 param)
#define INNER_HAS_PTHREAD_SETNAME_NP_NAME 1
#else
#define INNER_HAS_PTHREAD_SETNAME_NP_NAME 0
#endif
#else
#define INNER_HAS_PTHREAD_SETNAME_NP_NAME 0
#endif // defined(__APPLE__)

namespace utils {

namespace {

static pthread_t stdTidToPthreadId(std::thread::id tid) {
  static_assert(std::is_same<pthread_t, std::thread::native_handle_type>::value,
                "This assumes that the native handle type is pthread_t");
  static_assert(
      sizeof(std::thread::native_handle_type) == sizeof(std::thread::id),
      "This assumes std::thread::id is a thin wrapper around "
      "std::thread::native_handle_type, but that doesn't appear to be true.");
  // In most implementations, std::thread::id is a thin wrapper around
  // std::thread::native_handle_type, which means we can do unsafe things to
  // extract it.
  pthread_t id;
  std::memcpy(&id, &tid, sizeof(id));
  return id;
}

} // namespace

bool canSetCurrentThreadName() {
#if INNER_HAS_PTHREAD_SETNAME_NP_THREAD_NAME ||                                \
    INNER_HAS_PTHREAD_SETNAME_NP_NAME || INNER_DETAIL_HAS_PRCTL_PR_SET_NAME
  return true;
#else
  return false;
#endif
}

bool canSetOtherThreadName() {
#if (INNER_HAS_PTHREAD_SETNAME_NP_THREAD_NAME)
  return true;
#else
  return false;
#endif
}

static constexpr size_t MaxThreadNameLength = 16;

static std::optional<std::string> getPThreadName(pthread_t pid) {
#if (INNER_HAS_PTHREAD_SETNAME_NP_THREAD_NAME ||                               \
     INNER_HAS_PTHREAD_SETNAME_NP_NAME)
  // Android NDK does not yet support pthread_getname_np.
  std::array<char, MaxThreadNameLength> buf;
  if (pthread_getname_np(pid, buf.data(), buf.size()) == 0) {
    return std::string(buf.data());
  }
#endif
  (void)pid;
  return std::nullopt;
}

std::optional<std::string> getThreadName(std::thread::id id) {
#if (INNER_HAS_PTHREAD_SETNAME_NP_THREAD_NAME ||                               \
     INNER_HAS_PTHREAD_SETNAME_NP_NAME)
  // Android NDK does not yet support pthread_getname_np.
  if (id != std::thread::id()) {
    return getPThreadName(stdTidToPthreadId(id));
  }
#elif INNER_DETAIL_HAS_PRCTL_PR_SET_NAME
  std::array<char, kMaxThreadNameLength> buf;
  if (id == std::this_thread::get_id() &&
      prctl(PR_GET_NAME, buf.data(), 0L, 0L, 0L) == 0) {
    return std::string(buf.data());
  }
#endif
  (void)id;
  return std::nullopt;
} // namespace utils

std::optional<std::string> getCurrentThreadName() {
  return getPThreadName(pthread_self());
}

bool setThreadName(pthread_t pid, std::string_view name) {
  name = name.substr(0, MaxThreadNameLength - 1);
  char buf[MaxThreadNameLength] = {};
  std::memcpy(buf, name.data(), name.size());
#if INNER_HAS_PTHREAD_SETNAME_NP_THREAD_NAME
  return 0 == pthread_setname_np(pid, buf);
#else
#if INNER_HAS_PTHREAD_SETNAME_NP_NAME
  // Since macOS 10.6 and iOS 3.2 it is possible for a thread
  // to set its own name using pthread, but
  // not that of some other thread.
  if (pthread_equal(pthread_self(), pid)) {
    return 0 == pthread_setname_np(buf);
  }
#elif INNER_DETAIL_HAS_PRCTL_PR_SET_NAME
  // for Android prctl is used instead of pthread_setname_np
  // if Android NDK version is older than API level 9.
  if (pthread_equal(pthread_self(), pid)) {
    return 0 == prctl(PR_SET_NAME, buf, 0L, 0L, 0L);
  }
#else
  (void)pid;
#endif
  return false;
#endif // !INNER_HAS_PTHREAD_SETNAME_NP_THREAD_NAME
}

bool setThreadName(std::thread::id tid, std::string_view name) {
  return setThreadName(stdTidToPthreadId(tid), name);
}

bool setThreadName(std::string_view name) {
  return setThreadName(pthread_self(), name);
}

} // namespace utils