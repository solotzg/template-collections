
#include "math/Template/prime_factors.hpp"

struct Solve {
  Solve(size_t maxn) : maxn_(maxn), prime_(maxn_) {
    LOG_INFO("finish init. maxn: {}", maxn);
  }

  void dfs(int *data, int max_len, uint64_t pre, uint64_t pre_cnt, size_t index,
           uint64_t a, uint64_t b, uint64_t &res) {
    if (index == max_len) {
      if (pre == 1)
        return;
      if (pre_cnt & 1) {
        res += b / pre - a / pre;
      } else {
        res -= b / pre - a / pre;
      }
      return;
    }
    dfs(data, max_len, pre, pre_cnt, index + 1, a, b, res);
    dfs(data, max_len, pre * data[index], pre_cnt + 1, index + 1, a, b, res);
  }

  uint64_t count_no_prime_range(uint64_t n) {
    int data[20];
    int len = 0;
    prime_.Decompose(n, [&](uint32_t f, uint32_t) { data[len++] = f; });
    uint64_t res = 0;
    dfs(data, len, 1, 0, 0, n, maxn_, res);
    return res;
  }

  uint64_t solve() {
    constexpr uint64_t mod = 1e9 + 7;
    uint64_t res = 1;
    constexpr uint64_t step = 1e6;
    _ranged_step(bg, 1, maxn_, step) {
      auto ed = std::min(bg + step - 1, maxn_);
      repd(i, bg, ed) {
        auto n = count_no_prime_range(i);
        auto pn = (maxn_ - i - n);
        auto o = utils::fast_pow(i, pn, mod);
        res = res * o % mod;
      }
      LOG_INFO("resolved {}", ed);
    }
    return res;
  }

private:
  uint64_t maxn_;
  PrimeForDivide<> prime_;
};

namespace {
constexpr uint64_t mod = 1e9 + 7;

uint64_t g(uint64_t x) {
  uint64_t res = 1;
  rep(i, 2, x) {
    if (std::gcd(i, x) == 1) {
      res = res * i % mod;
    }
  }
  return res;
}

uint64_t bruce_force_calc(uint64_t n) {
  uint64_t res = 1;
  repd(x, 2, n) { res = res * g(x) % mod; }
  return res;
}
} // namespace

int main() {
  LOG_INFO("start");
  uint64_t n = 1e8;
  auto s = Solve(n);
  auto res = s.solve();
  MSGLN(res);
  ASSERT(bruce_force_calc(n), res);
}