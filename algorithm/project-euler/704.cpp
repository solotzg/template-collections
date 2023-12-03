#include <utils/utils.h>

uint64_t A119387(uint64_t n) {
  n += 1;
  return 64 - 1 - std::countl_zero(n) - std::countr_zero(n);
}

uint64_t Solve(uint64_t maxn) {
  SHOW_TIME_COST(__FUNCTION__);

  uint64_t res = A119387(maxn);
  maxn++;
  VI bin;
  {
    for (auto x = maxn; x; x >>= 1) {
      bin.emplace_back(x & 1);
    }
  }
  rep(i, 0, bin.size() - 1) {
    if (!bin[i])
      continue;
    bin[i] -= 1;
    auto pre_len = bin.size() - i;

    rp(last_one, i) {
      auto u = pre_len + (i - last_one) - 1;
      u *= utils::fast_pow(2, i - last_one - 1);
      res += u;
    }

    auto nxt = i;
    rep(j, i + 1, bin.size()) {
      if (bin[j]) {
        nxt = j;
        break;
      }
    }

    res += bin.size() - nxt - 1;

    bin[i] += 1;
  }
  {
    rep(first_one, 0, bin.size() - 1) {
      repd(last_one, 0, first_one) {
        auto u = first_one - last_one + 1 - 1;
        uint64_t m = 1;
        if (first_one != last_one)
          m = first_one - last_one - 1;
        u *= utils::fast_pow(2, m);
        res += u;
      }
    }
  }
  return res;
}

uint64_t SolveBruceForce(uint64_t maxn) {
  SHOW_TIME_COST(__FUNCTION__);

  uint64_t r = 0;
  repd(n, 1, maxn) { r += A119387(n); }
  return r;
}

int main() {
  constexpr uint64_t n = 1e16;
  auto res = Solve(n);
  MSGLN(res);
  DEBUG_SCOPE({ ASSERT_EQ(res, SolveBruceForce(n)); });
}