#include "utils/work.h"
#include <cassert>
#include <vector>

int64_t solve(int n) {
  ModuloOperator<int32_t, int64_t, 1020202009> modulo_;

  std::vector<int32_t> dp;
  dp.resize(n + 2, 0);
  std::vector<int32_t> comb;
  comb.resize(n + 2, 0);

  dp[0] = 1;
  comb[0] = 1;

  for (int i = 0; i < n; ++i) {
    auto &d = dp[i + 1];
    for (int k = 0; k <= i; k += 2) {
      auto p = modulo_.mul(modulo_.mul(comb[k], dp[k]), dp[i - k]);
      d = modulo_.add(d, p);
    }
    //
    for (int j = i + 1; j != 0; --j) {
      comb[j] = modulo_.add(comb[j - 1], comb[j]);
    }
  }

  return dp[n];
}

int main() { MSGLN(solve(24680)); }