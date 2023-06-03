
struct Solve {
  Solve(size_t maxn) : maxn_(maxn) {
    LOG_INFO("maxn: {}", maxn);
    init();
    LOG_INFO("finish init");
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
    for (auto x = n; x != 1;) {
      if (one_factor_[x] == 0) {
        data[len++] = x;
        break;
      }
      auto p = one_factor_[x];
      do {
        x /= p;
      } while (x % p == 0);
      data[len++] = p;
    }
    uint64_t res = 0;
    dfs(data, len, 1, 0, 0, n, maxn_, res);
    return res;
  }

  uint64_t solve() {
    constexpr uint64_t mod = 1e9 + 7;
    uint64_t res = 1;
    repd(i, 2, maxn_) {
      auto n = count_no_prime_range(i);
      auto pn = (maxn_ - i - n);
      auto o = utils::fast_pow(i, pn, mod);
      res = res * o % mod;
      if (i % 1000000 == 0) {
        LOG_INFO("resolved {}", i);
      }
    }
    return res;
  }

private:
  void init() {
    uint64_t max_len_ = maxn_ + 1;
    one_factor_.resize(max_len_, 0);

    std::vector<uint64_t> primes;
    for (size_t i = 2; i < max_len_; ++i) {
      if (0 == one_factor_[i]) {
        primes.emplace_back(i);
      }
      for (const auto &e : primes) {
        auto p = e * i;
        if (p >= max_len_)
          break;
        one_factor_[p] = e;
        if (i % e == 0)
          break;
      }
    }
  }

private:
  uint64_t maxn_;
  std::vector<uint64_t> one_factor_;
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
  uint64_t n = 1e8;
  auto s = Solve(n);
  auto res = s.solve();
  MSGLN(res);
  ASSERT(bruce_force_calc(n), res);
}