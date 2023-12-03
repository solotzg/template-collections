#include <utils/thread-name.hpp>
#include <utils/utils.h>

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

void STDCoutGuard::Println(std::string_view msg) {
  lock_.RunWithMutexLock([&] {
    buffer_.resize(msg.size() + 1);
    std::memcpy(buffer_.data(), msg.data(), msg.size());
    buffer_.data()[msg.size()] = utils::CRLF;
    std::fwrite(buffer_.data(), 1, buffer_.size(), stdout);
  });
}

char *SerializeTimepoint(char *data, utils::SysSeconds &last_update_time_sec,
                         const SystemClock::time_point _time_point) {
  constexpr const char *millisec_format = "{:03d}";
  constexpr size_t millisec_bytes = 3;
  constexpr U64 max_milliseconds = 1000;
  using DataType = std::array<char, millisec_bytes>;
  using DataPtr = std::atomic<DataType *>;
  static std::array<DataPtr, max_milliseconds> milliseconds_map{};
  auto time_point_ms =
      std::chrono::time_point_cast<utils::Milliseconds>(_time_point);
  auto time_point_sec =
      std::chrono::time_point_cast<utils::Seconds>(_time_point);
  if (time_point_sec != last_update_time_sec) {
    fmt::format_to(data, "[{:%Y-%m-%d %T}]", time_point_ms);
    last_update_time_sec = time_point_sec;
    return data;
  }
  {
    auto &&millisec =
        time_point_ms.time_since_epoch().count() % max_milliseconds;
    auto ptr = milliseconds_map[millisec].load(std::memory_order_relaxed);
    if (!ptr) {
      auto *new_ptr = new DataType();
      fmt::format_to_n(new_ptr->data(), millisec_bytes, millisec_format,
                       millisec);
      if (!milliseconds_map[millisec].compare_exchange_strong(ptr, new_ptr)) {
        delete new_ptr;
      }
      ptr = new_ptr;
    }
    std::memcpy(data + utils::kLogTimePointSize - millisec_bytes - 1,
                ptr->data(), millisec_bytes);
  }
  return data;
}

const char *
ThreadLocalSerializeTimepoint(const SystemClock::time_point time_point) {
  thread_local std::array<char, utils::kLogTimePointSize> buff{};
  thread_local utils::SysSeconds last_update_time_sec{};
  return SerializeTimepoint(buff.data(), last_update_time_sec, time_point);
}

MutexLockWrap STDCoutGuard::lock_;
fmt::memory_buffer STDCoutGuard::buffer_;
utils::SysSeconds STDCoutGuard::last_update_time_sec_;

} // namespace utils
