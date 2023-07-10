struct Runner {
  static constexpr int64_t N = 100000;
#ifdef NDEBUG
  static constexpr bool enable_check = false;
#else
  static constexpr bool enable_check = true;
#endif

  int64_t calc_odd_no_5(int64_t n) {
    n %= N;
    if (mul_cache[n] != -1)
      return mul_cache[n];
    int64_t res = 1;
    for (int64_t i = 1; i <= n; i += 2) {
      if (i % 5 == 0)
        continue;
      res = res * i % N;
    }
    return mul_cache[n] = res;
  }

  int64_t calc_odd(int64_t n) {
    if (!n)
      return 1;
    if (auto it = cache.find(n); it != cache.end()) {
      auto y = it->second;
      return y;
    }
    auto res = calc_odd(n / 5) * calc_odd_no_5(n) % N;

    if constexpr (enable_check) {
      int64_t o = 1;
      for (int64_t i = 1; i <= n; i++) {
        if (i % 2 == 0)
          continue;
        auto x = i;
        while (x % 5 == 0)
          x /= 5;
        o = o * x % N;
      }
      assert(res == o);
    }

    return cache[n] = res;
  }

  int64_t calc_even(int64_t n) {
    auto res = calc_no_2_no_5(n / 2);

    if constexpr (enable_check) {
      int64_t o = 1;
      for (int64_t i = 1; i <= n; i++) {
        if (i % 2 == 0) {
          auto x = i;
          for (; x % 5 == 0; x /= 5) {
          }
          for (; x % 2 == 0; x /= 2) {
          }
          o = o * x % N;
        }
      }
      assert(o == res);
    }

    return res;
  }
  template <int64_t x> int64_t calc_x(int64_t n) {
    int64_t p = x;
    int64_t res = 0;
    for (; n >= p;) {
      res += n / p;
      p *= x;
    }
    return res;
  }

  int64_t calc_no_2_no_5(int64_t n) {
    if (!n)
      return 1;
    auto res = calc_odd(n);
    res = res * calc_even(n) % N;
    return res;
  }

  int64_t foo(int64_t n) {
    mul_cache.resize(N + 10, -1);
    auto c_2 = calc_x<2>(n);
    auto c_5 = calc_x<5>(n);
    c_2 -= c_5;

    auto res = calc_no_2_no_5(n) *
               OperatorWithModulo<int64_t, int64_t>::pow_mod(2, c_2, N) % N;

#ifndef NDEBUG
    {
      int c2_n = 0, c5_n{};
      int64_t o = 1;
      for (int64_t i = 1; i <= n; i++) {
        if (i % 2 == 0) {
          auto x = i;
          for (; x % 2 == 0; x /= 2) {
            c2_n++;
          }
        }
        if (i % 5 == 0) {
          auto x = i;
          for (; x % 5 == 0; x /= 5) {
            c5_n++;
          }
        }
        o = o * i;
        if (o % 10 == 0)
          o /= 10;
        o %= N;
      }
      assert(c2_n == calc_x<2>(n));
      assert(c5_n == calc_x<5>(n));

      assert(res == o);
    }
#endif
    return res;
  }
  void run(int64_t n) { MSGLN(foo(n)); }

  std::unordered_map<int64_t, int64_t> cache;
  std::vector<int64_t> mul_cache;
};
int main() {
  SHOW_TIME_COST()
  Runner runner{};
  assert(runner.foo(9) == 36288);
  assert(runner.foo(10) == 36288);
  assert(runner.foo(20) == 17664);
  runner.run(1000000000000ll);
}