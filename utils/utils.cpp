#include "utils.h"
#include "thread-name.hpp"

namespace utils {

TimeCost::TimeCost(std::string_view label, bool auto_show)
    : label_(label), start_(Clock::now()), auto_show_(auto_show) {}

void TimeCost::Show(const char *prefix) const {
  auto end = Clock::now();

  FMSGLN("[{}]{}[time cost: {}]", label_,
         prefix ? fmt::format("[{}]", prefix) : "",
         std::chrono::duration_cast<Milliseconds>(end - start_));
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
    if (c >= 'a' && c <= 'z') {
      const int diff = 'A' - 'a';
      c += diff;
    }
  }
}

} // namespace utils
