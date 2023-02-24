#include "build_info.h"
#include "version.h"

namespace BuildInfo {
const char *ReleaseVersion() { return RELEASE_VERSION; }
const char *BuildTime() { return BUILD_TIME; }
const char *GitBranch() { return GIT_BRANCH; }
const char *GitHash() { return GIT_HASH; }
} // namespace BuildInfo