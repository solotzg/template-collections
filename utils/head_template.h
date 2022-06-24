#pragma once

#include "head_define.h"

template <typename T, typename UP> struct OperatorWithModulo {
  static inline T mul_mod(T a, T b, T mod) {
    if constexpr (sizeof(T) < sizeof(UP)) {
      return UP(a) * b % mod;
    } else {
      T res{};
      while (b) {
        if (b & 1)
          res = add_mod(res, a, mod);
        a = add_mod(a, a, mod);
        b >>= 1;
      }
      return res;
    }
  }
  static inline void smul_mod(T &a, T b, T mod) { a = mul_mod(a, b, mod); }
  static inline T add_mod(T a, T b, T mod) {
    if constexpr (std::numeric_limits<T>::min() == 0)
      return (a += b) >= mod ? a - mod : (a < 0 ? a + mod : a);
    else
      return (a += b) >= mod ? a - mod : a;
  }

  static inline void sadd_mod(T &a, T b, T mod) { a = add_mod(a, b, mod); }

  static inline T pow_mod(T a, T b, T mod) {
    T r = 1, p = a;
    while (b) {
      if (b & 1)
        r = mul_mod(r, p, mod);
      p = mul_mod(p, p, mod);
      b >>= 1;
    }
    return r;
  }
};

template <typename T, typename UP, const T MOD = 1000000007>
struct ModuloOperator : OperatorWithModulo<T, UP> {
  using Base = OperatorWithModulo<T, UP>;
  inline T mul(T a, T b) { return Base::mul_mod(a, b, MOD); }
  inline void smul(T &a, T b) { return Base::smul_mod(a, b, MOD); }
  inline T add(T a, T b) { return Base::add_mod(a, b, MOD); }
  inline void sadd(T &a, T b) { return Base::sadd_mod(a, b, MOD); }
  inline T pow(T a, T b) { return Base::pow_mod(a, b, MOD); }
};

template <size_t N> struct Comb {
  void init() {
    rp(i, N) {
      comb[i][0] = comb[i][i] = 1;
      rep(j, 1, i) { comb[i][j] = add_mod(comb[i - 1][j - 1], comb[i - 1][j]); }
    }
  }
  int get_comb(int n, int k) {
    assert(k <= n);
    assert(n < N);
    assert(k < N);
    return comb[n][k];
  }

  int comb[N][N];
};

template <size_t N> struct Factorial {
  void init(int64_t mod) {
    A[0] = RA[0] = 1;
    rep(i, 1, N) A[i] = mul_mod(A[i - 1], i), RA[i] = pow(A[i], mod - 2);
  }
  int get_comb(int n, int k) {
    assert(k <= n);
    assert(n < N);
    assert(k < N);
    return mul_mod(A[n], mul_mod(RA[k], RA[n - k]));
  }
  int A[N], RA[N];
};

template <class T, class U> T cast(U x) {
  T y;
  std::ostringstream a;
  a << x;
  std::istringstream b(a.str());
  b >> y;
  return y;
}

template <class T>
std::vector<T> split(const std::string &s, const std::string &x = " ") {
  std::vector<T> r;
  rp(i, s.size()) {
    std::string c;
    while (i < (int)s.size() && x.find(s[i]) == std::string::npos)
      c += s[i++];
    if (c.size())
      r.push_back(cast<T>(c));
  }
  return r;
}

template <class T> inline void tin(T &x) {
  int sg = 1;
  char c;
  while (((c = getchar()) < '0' || c > '9') && c != '-')
    ;
  c == '-' ? (sg = -1, x = 0) : (x = c - '0');
  while ((c = getchar()) >= '0' && c <= '9')
    x = x * 10 + c - '0';
  x *= sg;
}
