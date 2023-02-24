#pragma once

namespace BuildInfo {
const char *ReleaseVersion();
const char *BuildTime();
const char *GitBranch();
const char *GitHash();
} // namespace BuildInfo