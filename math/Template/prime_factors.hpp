#pragma once

#include "utils/utils.h"

template <typename T> struct Factor {
  T num_;
  size_t cnt_;
  Factor(T num, size_t cnt) : num_(num), cnt_(cnt) {}
};

template <typename T> struct Factors : std::vector<Factor<T>> {
  template <typename F> void dfs(F &&f) { dfs(0, 1, std::move(f)); }
  template <typename F> void dfs(size_t pos, T val, F &&f) {
    if (pos == this->size()) {
      f(val);
      return;
    }
    for (int i = 0; i <= this->data()[pos].cnt_; ++i) {
      dfs(pos + 1, val, std::move(f));
      val *= this->data()[pos].num_;
    }
  }
};

template <typename T> struct FactorMap {
  using Factors = Factors<T>;
  using Self = FactorMap;

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

  double ToCompVal() const {
    return std::accumulate(data_.begin(), data_.end(), 0.0,
                           [](auto res, auto &&x) {
                             auto &&[k, v] = x;
                             return res + v * std::log(k);
                           });
  }

  using Data = std::unordered_map<T, size_t>;

  Data &data() { return data_; }
  const Data &data() const { return data_; }

private:
  Data data_;
};

template <typename T = uint32_t> struct PrimeForDivide {
  using Factors = Factors<T>;

  PrimeForDivide(size_t maxn) : maxn_(maxn) { InitPrimes(); }

  T GetOneFactor(T n) {
    ASSERT_LE(n, maxn_);
    auto p = one_factor_[n];
    return p == 0 ? n : p;
  }

  template <typename F> void Decompose(T n, F &&cb) {
    ASSERT_LE(n, maxn_);
    while (n > 1) {
      T f = GetOneFactor(n);
      uint32_t c = 0;
      do {
        n /= f, ++c;
      } while (n % f == 0);
      cb(f, c);
    }
  }

  void Decompose(T x, Factors &res) {
    res.clear();
    Decompose(x, [&](T x, size_t cnt) { res.emplace_back(x, cnt); });
  }

  Factors Decompose(T x) {
    Factors res;
    Decompose(x, res);
    return res;
  }

  bool IsPrime(T n) const {
    ASSERT_LE(n, maxn_);
    return one_factor_[n] == 0;
  }

  void InitPrimes() {
    one_factor_.resize(maxn_ + 1, 0);
    repd(i, 2, maxn_) {
      if (IsPrime(i))
        primes_.emplace_back(i);
      for (const auto &e : primes_) {
        auto p = e * i;
        if (p > maxn_)
          break;
        ASSERT_EQ(one_factor_[p], 0);
        one_factor_[p] = e;
        if (i % e == 0)
          break;
      }
    }
  }

  size_t maxn_;
  std::vector<T> one_factor_;
  std::vector<T> primes_;
};

struct Prime {
  using T = uint64_t;
  using Factors = Factors<T>;
  using FactorMap = FactorMap<T>;

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

  void Decompose(T x, Factors &res) {
    res.clear();
    Decompose(x, [&](T x, size_t cnt) { res.emplace_back(x, cnt); });
  }

  template <typename F> void Decompose(T x, F &&f) {
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
  Factors Decompose(T x) {
    Factors res;
    Decompose(x, res);
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

#ifndef NDEBUG
namespace tests {
static void _test_prime_factors() {
  typedef uint64_t T;
  T a = 20100224546;
  auto prime = Prime::GenPrimeWithMaxNum(1e9);
  assert(!Prime::IsPrime(a));
  {
    auto factors = prime.Decompose(a);
    T sum = 1;
    for (const auto &[k, v] : factors) {
      sum *= utils::fast_pow(k, v);
      assert(Prime::IsPrime(k));
    }
    assert(sum == a);
  }
  {
    auto factors = prime.Decompose(240);
    size_t cnt = 0;
    factors.dfs([&](T p) { cnt += 1; });
    assert(cnt == 20);
  }
  {
    Prime::FactorMap factors;
    prime.Decompose(a, [&](int64_t x, size_t cnt) { factors.add(x, cnt); });
    T sum = 1;
    for (const auto &[k, v] : factors.data()) {
      sum *= utils::fast_pow(k, v);
      assert(Prime::IsPrime(k));
    }
    assert(sum == a);
  }
  {
    Prime::FactorMap factors = prime.Decompose(a);
    T sum = 1;
    for (const auto &[k, v] : factors.data()) {
      sum *= utils::fast_pow(k, v);
      assert(Prime::IsPrime(k));
    }
    assert(sum == a);
  }

  a = 1;
  for (int i = 0; i < prime.primes().size() && a < 1e10;
       a *= prime.primes()[i++])
    ;
  {
    auto factors = prime.Decompose(a);
    T sum = 1;
    for (const auto &[k, v] : factors) {
      sum *= utils::fast_pow(k, v);
    }
    assert(sum == a);
  }
  {
    int x = 656744;
    Prime::FactorMap factors;
    PrimeForDivide p(x);
    p.Decompose(x, [&](int f, int c) { factors.add(f, c); });
    int sum = 1;
    for (const auto &[k, v] : factors.data()) {
      sum *= utils::fast_pow(k, v);
    }
    assert(sum == x);
  }
  {
    PrimeForDivide p(2099);
    p.Decompose(2099, [](int f, int c) {
      ASSERT_EQ(f, 2099);
      ASSERT_EQ(c, 1);
    });
    ASSERT(p.IsPrime(2099));
  }
}
} // namespace tests
#endif