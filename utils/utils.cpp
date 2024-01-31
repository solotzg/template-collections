#include <utils/thread-name.hpp>
#include <utils/utils.h>

namespace utils {

TimeCost::TimeCost(std::string_view label, bool auto_show)
    : label_(std::make_unique<std::string>(label)), start_(Clock::now()),
      auto_show_(auto_show) {}

void TimeCost::Show(const char *prefix) const {
  Show(Clock::now() - start_, prefix);
}

void TimeCost::Show(TimeCost::Clock::duration dur, const char *prefix) const {
  FMSGLN("[{}]{}[time cost: {}]", label(),
         prefix ? fmt::format("[{}]", prefix) : "",
         std::chrono::duration_cast<Milliseconds>(dur));
}

std::string_view TimeCost::label() const { return (*label_); }

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

static std::pair<utils::SysMilliseconds, U64>
into_sec_remain_millisec(utils::SysMilliseconds time_point) {
  auto time_point_sec = std::chrono::time_point_cast<utils::Milliseconds>(
      std::chrono::time_point_cast<utils::Seconds>(time_point));
  auto &&millisec = (time_point - time_point_sec).count();

  return {time_point_sec, millisec};
}

void SerializeTimepoint(char *data,
                        utils::SysMilliseconds &last_update_time_sec,
                        const utils::SysMilliseconds time_point_ms) {
  constexpr size_t millisec_bytes = 3;
  constexpr U64 max_milliseconds = 1000;
  using DataType = std::array<char, millisec_bytes>;
  using DataMap = std::array<DataType, max_milliseconds>;
  static const auto &&milliseconds_map = []() -> DataMap {
    constexpr const char *millisec_format = "{:03d}";
    DataMap map{};
    rp(msec, max_milliseconds) {
      fmt::format_to_n(map[msec].data(), millisec_bytes, millisec_format,
                       U64(msec));
    }
    return map;
  }();
  auto &&[time_point_sec, millisec] = into_sec_remain_millisec(time_point_ms);
  if (time_point_sec != last_update_time_sec) {
    fmt::format_to(data, "[" FMT_TIMEPOINT_MICROSEC "]", time_point_ms);
    last_update_time_sec = time_point_sec;
    return;
  }
  {
    std::memcpy(data + utils::kLogTimePointSize - millisec_bytes - 1,
                milliseconds_map[millisec].data(), millisec_bytes);
  }
}

MutexLockWrap global_stdout_lock;

} // namespace utils
