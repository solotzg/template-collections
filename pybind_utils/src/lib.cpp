#include "prime_helper.h"
#include "pybind_utils/pch.h"

PYBIND11_MODULE(pybind_utils, m) {
  m.doc() = "pybind11 plugin `pybind_utils`";
  m.def("get_tid", &utils::get_tid);
  m.def("gen_prime_helper_with_maxlen", gen_prime_helper_with_maxlen);
  m.def("gen_prime_helper_with_maxnum", gen_prime_helper_with_maxnum);
  m.def("prime_helper_init_pi_small", prime_helper_init_pi_small);
  m.def("prime_helper_pi", prime_helper_pi);
  m.def("prime_helper_is_prime", prime_helper_is_prime);
  m.def("destroy_prime_helper", destroy_prime_helper);
  m.def("prime_helper_decompose", prime_helper_decompose);
}
