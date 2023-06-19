#include "pybind_utils/pch.h"

namespace {
std::string to_log_msg(std::string_view &&msg) {
  auto &&s = TO_LOG_MSG((msg));
  return s;
}
} // namespace

PYBIND11_MODULE(pybind_utils, m) {
  m.doc() = "pybind11 plugin `pybind_utils`";
  m.def("to_log_msg", &to_log_msg,
        "A function that make message into log message");
}
