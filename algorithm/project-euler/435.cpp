
#include "algorithm/data_structure/Template/matrix_operation.hpp"

constexpr uint64_t get_mod() {
  uint64_t r = 1;
  for (uint64_t x = 1; x <= 15; ++x)
    r *= x;
  return r;
}
using M = Matrix<int64_t, 2, 2>;
using OP = utils::OperatorWithModulo<int64_t, I128>;

int64_t F(int64_t n, int64_t x) {
  auto b = x * x + x - 1;
  auto mod = get_mod() * b;

  int64_t a1 =
      OP::mul_mod(M::Fibonacci<I128>(n, mod), OP::pow_mod(x, n + 2, mod), mod);
  int64_t a2 = OP::mul_mod(M::Fibonacci<I128>(n + 1, mod),
                           OP::pow_mod(x, n + 1, mod), mod);
  auto a = OP::add_mod(a1, a2, mod);
  a = OP::add_mod(a, -x, mod);
  ASSERT_EQ(a % b, 0);
  a /= b;
  return a;
}

int main(int argc, char **argv) {
  int64_t n = 1e15;
  int64_t res = 0;
  assert(F(7, 11) == 268357683);
  for (int64_t x = 1; x <= 100; ++x) {
    res = OP::add_mod(res, F(n, x), get_mod());
  }
  MSGLN(res);
}
