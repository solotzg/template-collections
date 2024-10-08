#pragma once

#include <utils/utils.h>

template <typename T> struct Factor {
  T num;
  size_t cnt;
  Factor(T _num, size_t _cnt) : num(_num), cnt(_cnt) {}
};

template <typename T> struct Factors : std::vector<Factor<T>> {
  template <typename F> void dfs(F &&f) { dfs(0, 1, std::move(f)); }
  template <typename F> void dfs(size_t pos, T val, F &&f) {
    if (pos == this->size()) {
      f(val);
      return;
    }
    for (int i = 0; i <= this->data()[pos].cnt; ++i) {
      dfs(pos + 1, val, std::move(f));
      val *= this->data()[pos].num;
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

namespace {
inline uint64_t icbrt(uint64_t n) {
  uint64_t s = std::pow(n, 1.0 / 3);
  while ((s + 1) * (s + 1) * (s + 1) <= n) {
    s += 1;
  }
  while (s * s * s > n) {
    s -= 1;
  }
  return s;
}
} // namespace

struct PrimeHelper {
  using T = uint64_t;
  using Factors = Factors<T>;
  using FactorMap = FactorMap<T>;
  using TypeIsPrime = std::vector<bool>;
  using TypePrimes = std::vector<T>;
  using TypePiSmall = std::vector<uint32_t>;

  PrimeHelper(const PrimeHelper &) = delete;
  PrimeHelper(PrimeHelper &&src) = default;

  static PrimeHelper GenPrimeHelperWithMaxNum(T max_num) {
    return PrimeHelper(
        std::max(static_cast<size_t>(std::sqrt(max_num)), size_t(1)));
  }
  static PrimeHelper GenPrimeHelperWithMaxLen(T max_len) {
    return PrimeHelper(max_len);
  }

  static bool IsPrimeBruceForce(T x) {
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

  void InitPiSmall() {
    pi_small_.resize(max_len_, 0);
    uint32_t prime_cnt = 0;
    rep(i, 2, max_len_) {
      if (is_prime_[i])
        prime_cnt++;
      pi_small_[i] = prime_cnt;
    }
  }

  uint64_t Pi(T n) {
    if (n < max_len_)
      return pi_small_[n];
    uint64_t r = icbrt(n);
    uint64_t k = std::sqrt(n);
    uint64_t pr = Pi(r);
    uint64_t pk = Pi(k);
    auto mu = pk - pr;
    auto s = PiFunc(pr - 1, n) - 1 + pr - mu * (mu + 1) / 2 + mu * pk;
    rep(i, pr, pk) { s -= Pi(n / primes_[i]); }
    return s;
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

  std::vector<std::pair<uint64_t, uint64_t>> DecomposeSTL(T num) {
    std::vector<std::pair<uint64_t, uint64_t>> res;
    Decompose(num, [&](uint64_t f, uint64_t cnt) { res.emplace_back(f, cnt); });
    return res;
  }

  bool IsPrime(T x) {
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
  uint64_t PiFunc(int64_t i, T n) {
    if (i < 0)
      return n;
    if (n == 0)
      return 0;
    if (primes_[i] > n)
      return 1;
    auto s = n;
    while (i >= 0) {
      s -= PiFunc(i - 1, n / primes_[i]);
      i -= 1;
    }
    return s;
  }

  PrimeHelper(size_t max_len) {
    max_len_ = max_len + 1;
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

  friend class tests::__inner__;

private:
  size_t max_len_;
  TypeIsPrime is_prime_;
  TypePrimes primes_;
  TypePiSmall pi_small_;
};
