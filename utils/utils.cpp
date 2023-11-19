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

void STDCoutGuard::PrintWithTimepointPrefix(std::string_view str) {
  std::array<char, utils::kLogTimePointSize> buff;
  lock_.RunWithMutexLock([&] {
    SerializeTimepoint(buff.data(), SystemClock::now());
    std::cout << std::string_view{buff.data(), buff.size()} << str << std::endl;
  });
}

MutexLockWrap STDCoutGuard::lock_{};

} // namespace utils
