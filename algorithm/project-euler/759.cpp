#include "utils/utils.h"

using D = std::tuple<uint64_t, uint64_t, uint64_t>;
static_assert(sizeof(uint64_t) == 8);

constexpr uint64_t MOD = 1e9 + 7;

uint64_t foo(uint64_t x) {
  auto f = x % MOD * std::popcount(x) % MOD;
  return f;
}
uint64_t foo2(uint64_t x) {
  auto f = foo(x);
  return f * f % MOD;
}

uint64_t func_s(const size_t n) {
  uint64_t res = 0;
  repd(i, 1, n) {
    res += foo2(i);
    res %= MOD;
  }
  return res;
}

struct Cache : std::array<std::array<D, 100>, 100> {
  Cache() {
    for (auto &&x : *this) {
      x.fill({-1, -1, -1});
    }
  }
  bool has(size_t n, size_t bit_cnt) const {
    auto &&[x, _, _2] = (*this)[n][bit_cnt];
    return x != -1;
  }
};

// COUNT, SUM x^2, SUM x
D dfs(size_t n, size_t bit_cnt) {
  static Cache cache{};

  if (bit_cnt > n)
    return {0, 0, 0};
  if (n == 0) {
    return {1, 0, 0};
  }
  if (n == 1) {
    if (bit_cnt)
      return {1, 1, 1};
    return {1, 0, 0};
  }

  if (cache.has(n, bit_cnt))
    return cache[n][bit_cnt];

  uint64_t res_n{};
  uint64_t res_x2{}, res_x{};
  auto left_n = n / 2;
  auto right_n = n - left_n;
  repd(left, 0, bit_cnt) {
    auto right = bit_cnt - left;
    auto &&[cx, x2, x] = dfs(left_n, left);
    auto &&[cy, y2, y] = dfs(right_n, right);
    res_n += cx * cy % MOD;
    res_n %= MOD;
    res_x2 += x2 * fast_pow(2, 2 * right_n, MOD) % MOD * cy % MOD;
    res_x2 %= MOD;
    res_x2 += y2 * cx % MOD;
    res_x2 %= MOD;
    res_x2 += x * fast_pow(2, 1 + right_n, MOD) % MOD * y % MOD;
    res_x2 %= MOD;
    res_x += x * fast_pow(2, right_n, MOD) % MOD * cy % MOD;
    res_x %= MOD;
    res_x += y * cx % MOD;
    res_x %= MOD;
  }
  RUNTIME_ASSERT(res_n < MOD);
  RUNTIME_ASSERT(res_x2 < MOD);
  RUNTIME_ASSERT(res_x < MOD);
  return cache[n][bit_cnt] = {res_n, res_x2, res_x};
}

uint64_t func_s2(const uint64_t n) {
  std::vector<uint64_t> v{};
  {
    uint64_t d = n;
    for (; d;) {
      v.emplace_back(d & 1);
      d /= 2;
    }
  }
  const auto bit_of_n = std::popcount(n);
  auto bit_of_np = bit_of_n;
  uint64_t np = n;
  uint64_t ans{};
  rep(i, 0, v.size()) {
    if (v[i] == 0) {
      np >>= 1;
      continue;
    } else {
      bit_of_np--;
      np -= 1;
    }
    uint64_t cx = 1, x = np % MOD;
    uint64_t x2 = (np % MOD) * (np % MOD) % MOD;
    auto right_n = i;

    repd(t, 0, right_n) {
      uint64_t res_x2{};
      auto &&[cy, y2, y] = dfs(right_n, t);
      res_x2 += x2 * fast_pow(2, 2 * right_n, MOD) % MOD * cy % MOD;
      res_x2 %= MOD;
      res_x2 += y2 * cx % MOD;
      res_x2 %= MOD;
      res_x2 += x * fast_pow(2, 1 + right_n, MOD) % MOD * y % MOD;
      res_x2 %= MOD;
      ans += res_x2 * (t + bit_of_np) % MOD * (t + bit_of_np) % MOD;
      ans %= MOD;
    }
    np >>= 1;
  }
  ans = (ans + foo2(n)) % MOD;

  // ASSERT_EQ(ans, func_s(n));
  return ans;
}

int main() {
  SHOW_TIME_COST()
#define F func_s2
  uint64_t n = 1e16;
  MSGLN(F(n));
#undef F
}