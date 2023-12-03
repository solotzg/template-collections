#include <algorithm/math/Template/prime_factors.hpp>
#include <pybind_utils/pch.h>

namespace py = pybind11;

PYBIND11_MODULE(pybind_utils, m) {
  m.doc() = "pybind11 plugin `pybind_utils`";
  m.def("get_tid", &utils::get_tid);
  py::class_<PrimeHelper>(m, "PrimeHelper")
      .def(py::init(&PrimeHelper::GenPrimeHelperWithMaxLen))
      .def_static("is_prime_bruceforce", &PrimeHelper::IsPrimeBruceForce)
      .def("is_prime", &PrimeHelper::IsPrime)
      .def("init_pi_small", &PrimeHelper::InitPiSmall)
      .def("pi", &PrimeHelper::Pi)
      .def("decompose", &PrimeHelper::DecomposeSTL)
      .def("primes", &PrimeHelper::primes);
  m.def("gen_prime_helper_with_maxnum", PrimeHelper::GenPrimeHelperWithMaxNum);
}
