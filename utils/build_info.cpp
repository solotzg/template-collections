#include <sstream>
#include <string_view>
#include <utils/build_info.h>
#include <utils/version.h>

namespace BuildInfo {
const char *ReleaseVersion() { return RELEASE_VERSION; }
const char *BuildTime() { return BUILD_TIME; }
const char *GitBranch() { return GIT_BRANCH; }
const char *GitHash() { return GIT_HASH; }
std::string GenFeatures() {
  constexpr std::string_view features[] = {
    CMAKE_CXX_STANDARD,
#if ENABLE_THINLTO
    "thinlto",
#endif
#if ARCHNATIVE
    "arch=native",
#endif
  };
  std::stringstream ss;
  for (auto &&s : features) {
    ss << s << ' ';
  }
  ss << char(0);
  return ss.str();
}
const char *Features() {
  static std::string features = GenFeatures();
  return features.data();
}
const char *Profile() { return CMAKE_BUILD_TYPE_UC; }

} // namespace BuildInfo