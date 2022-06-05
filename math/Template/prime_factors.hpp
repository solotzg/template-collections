#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <vector>

template <size_t N = 1000005> struct PrimeForDivide {
  // N is not very big
  int mxPrime[N];
  PrimeForDivide() { init_prime(); }
  void init_prime() {
    memset(mxPrime, 0, sizeof mxPrime);
    for (int i = 2; i < N; ++i) {
      if (mxPrime[i])
        continue;
      for (int j = i; j < N; j += i) {
        mxPrime[j] = i;
      }
    }
  }
  template <typename F> void divide(int x, F &&cb) {
    while (x > 1) {
      int j = mxPrime[x], c = 0;
      while (x % j == 0)
        x /= j, ++c;
      cb(j, c);
    }
  }
};

struct Prime {
  using T = uint64_t;
  static Prime GenPrimeWithMaxNum(T max_num) { return Prime(max_num, 0); }
  static Prime GenPrimeWithMaxLen(T max_len) { return Prime(0, max_len); }

  static bool IsPrime(T x) {
    assert(x > 1);
    for (T p = 2; p < x && p * p <= x; ++p) {
      if (x % p == 0)
        return false;
    }
    return true;
  }

  const std::vector<T> &primes() const { return primes_; }

  struct Factor {
    T num_;
    size_t cnt_;
    Factor(T num, size_t cnt) : num_(num), cnt_(cnt) {}
  };
  struct Factors : std::vector<Factor> {
    template <typename F> void dfs(F &&f) { dfs(0, 1, std::move(f)); }

  private:
    template <typename F> void dfs(size_t pos, T val, F &&f) {
      if (pos == size()) {
        f(val);
        return;
      }
      for (int i = 0; i <= data()[pos].cnt_; ++i) {
        dfs(pos + 1, val, std::move(f));
        val *= data()[pos].num_;
      }
    }
  };

  struct FactorMap {
    void add(T num, size_t cnt) { data_[num] += cnt; }
    FactorMap &operator*=(size_t cnt) {
      for (auto &e : data_) {
        e.second *= cnt;
      }
      return *this;
    }
    FactorMap &operator+=(const FactorMap &tar) {
      for (const auto &[k, v] : tar.data_) {
        add(k, v);
      }
      return *this;
    }
    FactorMap &operator+=(const Factors &tar) {
      for (const auto &[k, v] : tar) {
        add(k, v);
      }
      return *this;
    }
    FactorMap operator+(FactorMap tar) const {
      FactorMap res = *this;
      res += tar;
      return res;
    }
    FactorMap() {}
    FactorMap(Factors tar) {
      for (const auto &[k, v] : tar) {
        add(k, v);
      }
    }

    std::string show() const {
      std::stringstream ss;
      for (const auto &[k, v] : data_) {
        ss << "(" << k << ":" << v << ")";
      }
      return ss.str();
    }

    using Data = std::unordered_map<T, size_t>;

    Data &data() { return data_; }
    const Data &data() const { return data_; }

  private:
    Data data_;
  };

  void decompose(T x, Factors &res) {
    res.clear();
    decompose(x, [&](T x, size_t cnt) { res.emplace_back(x, cnt); });
  }

  template <typename F> void decompose(T x, F &&f) {
    for (const auto &n : primes_) {
      if (n * n > x)
        break;
      if (x % n == 0) {
        size_t cnt = 0;
        do {
          x /= n;
          cnt++;
        } while (x % n == 0);
        f(n, cnt);
      }
    }
    if (x != 1) {
      f(x, 1);
    }
  }
  Factors decompose(T x) {
    Factors res;
    decompose(x, res);
    return res;
  }

  bool isPrime(T x) {
    if (x < max_len_)
      return is_prime_[x];
    if ((x & 1) == 0)
      return false;
    for (const auto &n : primes_) {
      if (x % n == 0)
        return false;
    }
    return true;
  }

private:
  Prime(T max_num, size_t max_len) {
    if (max_len == 0) {
      max_len_ = std::max(static_cast<size_t>(std::sqrt(max_num)), size_t(1));
    } else {
      max_len_ = max_len;
    }
    max_len_++;
    is_prime_.resize(max_len_, 1);
    init();
  }

  void init() {
    is_prime_[0] = is_prime_[1] = 0;
    for (size_t i = 2; i < max_len_; ++i) {
      if (is_prime_[i]) {
        primes_.emplace_back(i);
      }
      for (const auto &e : primes_) {
        auto p = e * i;
        if (p >= max_len_)
          break;
        is_prime_[p] = 0;
        if (i % e == 0)
          break;
      }
    }
  }

private:
  size_t max_len_;
  std::vector<bool> is_prime_;
  std::vector<T> primes_;
};

inline uint64_t fast_pow(uint64_t a, uint64_t b) {
  uint64_t r = 1, p = a;
  while (b) {
    if (b & 1)
      r *= p;
    p *= p;
    b >>= 1;
  }
  return r;
}

static int _prime_tests() {
  typedef uint64_t T;
  T a = 20100224546;
  auto prime = Prime::GenPrimeWithMaxNum(1e9);
  assert(!Prime::IsPrime(a));
  {
    auto factors = prime.decompose(a);
    T sum = 1;
    for (const auto &[k, v] : factors) {
      sum *= fast_pow(k, v);
      assert(Prime::IsPrime(k));
    }
    assert(sum == a);
  }
  {
    auto factors = prime.decompose(240);
    size_t cnt = 0;
    factors.dfs([&](T p) { cnt += 1; });
    assert(cnt == 20);
  }
  {
    Prime::FactorMap factors;
    prime.decompose(a, [&](int64_t x, size_t cnt) { factors.add(x, cnt); });
    T sum = 1;
    for (const auto &[k, v] : factors.data()) {
      sum *= fast_pow(k, v);
      assert(Prime::IsPrime(k));
    }
    assert(sum == a);
  }
  {
    Prime::FactorMap factors = prime.decompose(a);
    T sum = 1;
    for (const auto &[k, v] : factors.data()) {
      sum *= fast_pow(k, v);
      assert(Prime::IsPrime(k));
    }
    assert(sum == a);
  }

  a = 1;
  for (int i = 0; i < prime.primes().size() && a < 1e10;
       a *= prime.primes()[i++])
    ;
  {
    auto factors = prime.decompose(a);
    T sum = 1;
    for (const auto &[k, v] : factors) {
      sum *= fast_pow(k, v);
    }
    assert(sum == a);
  }
  {
    int x = 656744;
    Prime::FactorMap factors;
    PrimeForDivide p;
    p.divide(x, [&](int f, int c) { factors.add(f, c); });
    int sum = 1;
    for (const auto &[k, v] : factors.data()) {
      sum *= fast_pow(k, v);
    }
    assert(sum == x);
  }
  return 0;
}