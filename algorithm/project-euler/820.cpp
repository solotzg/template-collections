#include <utils/utils.h>

int64_t D(int64_t n, int64_t k) {
  int64_t d = OperatorWithModulo<int32_t, int64_t>::pow_mod(10, n - 1, k);
  return d * 10 / k;
}

int64_t S(int64_t n) {
  int64_t res{};
  repd(k, 1, n) {
    auto x = D(n, k);
    res += x;

    DEBUG_MSGLN("n", n, "k", k, ":", x);
  }
  return res;
}

int main() {
  SHOW_TIME_COST()
  MSGLN(S(1e7));
}