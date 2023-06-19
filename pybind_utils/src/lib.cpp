#include "prime_helper.h"
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
  m.def("gen_prime_helper_with_maxlen", gen_prime_helper_with_maxlen);
  m.def("gen_prime_helper_with_maxnum", gen_prime_helper_with_maxnum);
  m.def("prime_helper_init_pi_small", prime_helper_init_pi_small);
  m.def("prime_helper_pi", prime_helper_pi);
  m.def("prime_helper_is_prime", prime_helper_is_prime);
  m.def("destroy_prime_helper", destroy_prime_helper);
}
