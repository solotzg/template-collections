struct Test {
  std::vector<int> prime_cnt;
  std::vector<bool> not_squarefree;

  uint64_t run(uint64_t maxn) {
    size_t n = std::sqrt(maxn);

    prime_cnt.resize(n + 5, 0);
    not_squarefree.resize(n + 5, 0);

    uint64_t ret = 0;
    bool cut_last = true;

    for (uint64_t p = 2; p <= n; ++p) {
      if (prime_cnt[p])
        continue;
      for (uint64_t x = p; x <= n; x += p) {
        prime_cnt[x]++;
      }
      uint64_t p2 = p * p;
      for (uint64_t x = p2; x <= n; x += p2) {
        not_squarefree[x] = 1;
      }
      if (cut_last && maxn % p2 == 0) {
        cut_last = false;
      }
    }
    /**
    f(2*3*5) = g(2) + g(3) + g(5) - g(2*3) - g(2*5) - g(3*5) + g(2*3*5)
    C(n,1) - C(n,2) + C(n,3) - ... C(n,n)
      = 1 - (1-1)^n = 1 (n % 2 == 0)
      = (1-1)^n + 1 = 1 (n % 2 == 1)
    */
    for (uint64_t p = 2; p <= n; ++p) {
      if (not_squarefree[p])
        continue;
      auto p2 = p * p;
      auto cnt = (maxn) / p2;
      if (prime_cnt[p] & 1) {
        ret += cnt;
      } else {
        ret -= cnt;
      }
    }

    return maxn - ret - (cut_last ? 1 : 0);
  }
};
int main() {
  SHOW_TIME_COST;
  Test t{};

  // MSGLN(t.run(12));
  MSGLN(t.run(1ull << 50));
}