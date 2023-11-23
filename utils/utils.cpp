#include "utils.h"
#include "thread-name.hpp"

namespace utils {

TimeCost::TimeCost(std::string_view label, bool auto_show)
    : label_(label), start_(Clock::now()), auto_show_(auto_show) {}

void TimeCost::Show(const char *prefix) const {
  Show(Clock::now() - start_, prefix);
}

void TimeCost::Show(TimeCost::Clock::duration dur, const char *prefix) const {
  FMSGLN("[{}]{}[time cost: {}]", label_,
         prefix ? fmt::format("[{}]", prefix) : "",
         std::chrono::duration_cast<Milliseconds>(dur));
}

TimeCost::~TimeCost() {
  if (auto_show_)
    Show("END");
}

void TimeCost::Reset() { start_ = Clock::now(); }

TimeCost::Clock::duration TimeCost::Duration() { return Clock::now() - start_; }

TimeCost::Seconds TimeCost::DurationSec() {
  return IntoDurationSec(Duration());
}

TimeCost::Seconds
TimeCost::IntoDurationSec(const TimeCost::Clock::duration dur) {
  auto &&t = double(duration_cast<utils::Milliseconds>(dur).count()) / 1000;
  return t;
}

void ShowBuildInfo(std::ostream &os) {
  os << "Release Version: " << BuildInfo::ReleaseVersion() << std::endl
     << "Git Commit Hash: " << BuildInfo::GitHash() << std::endl
     << "Git Branch:      " << BuildInfo::GitBranch() << std::endl
     << "Build Time:      " << BuildInfo::BuildTime() << std::endl
     << "Features:        " << BuildInfo::Features() << std::endl
     << "Profile:         " << BuildInfo::Profile() << std::endl;
}

std::string ToUpper(std::string_view s) {
  std::string res(s);
  ToUpper(res);
  return res;
}
void ToUpper(std::string &s) {
  for (auto &&c : s) {
    c += (c >= 'a' & c <= 'z') ? ('A' - 'a') : 0;
  }
}

std::string ToLower(std::string_view s) {
  std::string res(s);
  ToLower(res);
  return res;
}

void ToLower(std::string &s) {
  for (auto &&c : s) {
    c += (c >= 'A' & c <= 'Z') ? ('a' - 'A') : 0;
  }
}

void STDCoutGuard::Print(std::string_view s) {
  lock_.RunWithMutexLock([&] { std::cout << s << std::endl; });
}

const char *
ThreadLocalSerializeTimepoint(const SystemClock::time_point time_point) {
  thread_local std::array<char, utils::kLogTimePointSize> buff{};
  thread_local U64 last_update_time_sec{};
  using DataType = std::array<char, 3>;
  using DataPtr = std::atomic<DataType *>;
  static std::array<DataPtr, 1000> milliseconds_map{};
  constexpr size_t millisec_bytes = 3;
  constexpr const char *millisec_format = "{:03d}";

  auto &&sec = duration_cast<Seconds>(time_point.time_since_epoch()).count();
  if (sec != last_update_time_sec) {
    fmt::format_to_n(buff.data(), utils::kLogTimePointSize,
                     FMT_COMPILE("[{:%Y-%m-%d %H:%M:%S}.???]"), time_point);
    last_update_time_sec = sec;
  }
  {
    auto &&millisec =
        duration_cast<Milliseconds>(time_point.time_since_epoch()).count() %
        1000;
    auto ptr = milliseconds_map[millisec].load(std::memory_order_relaxed);
    if (!ptr) {
      auto *new_ptr = new DataType();
      fmt::format_to_n(new_ptr->data(), millisec_bytes,
                       FMT_COMPILE(millisec_format), millisec);
      if (!milliseconds_map[millisec].compare_exchange_strong(ptr, new_ptr)) {
        delete new_ptr;
      }
      ptr = new_ptr;
    }
    std::memcpy(buff.data() + utils::kLogTimePointSize - millisec_bytes - 1,
                ptr->data(), millisec_bytes);
  }
  return buff.data();
}

void STDCoutGuard::PrintWithTimepointPrefix(std::string_view str) {
  lock_.RunWithMutexLock([&] {
    auto &&now = SystemClock::now();
    auto buff = ThreadLocalSerializeTimepoint(now);
    std::cout << std::string_view{buff, kLogTimePointSize} << str << std::endl;
  });
}

MutexLockWrap STDCoutGuard::lock_{};

} // namespace utils
