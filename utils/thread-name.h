#pragma once

#include <array>
#include <optional>
#include <string>
#include <thread>

namespace utils {

bool canSetCurrentThreadName();
bool canSetOtherThreadName();
std::optional<std::string> getThreadName(std::thread::id tid);
std::optional<std::string> getCurrentThreadName();
bool setThreadName(std::thread::id tid, std::string_view name);
bool setThreadName(std::string_view name);
bool setThreadName(pthread_t pid, std::string_view name);

} // namespace utils