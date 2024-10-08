#pragma once

#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <utils/head_define.h>

namespace utils {

using SteadyClock = std::chrono::steady_clock;
using SystemClock = std::chrono::system_clock;

using Nanoseconds = std::chrono::nanoseconds;
using Microseconds = std::chrono::microseconds;
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;

using SysSeconds = std::chrono::sys_seconds;
using SysMilliseconds = std::chrono::sys_time<utils::Milliseconds>;

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
  static inline T &smul_mod(T &a, T b, T mod) { return a = mul_mod(a, b, mod); }
  static inline T add_mod(T a, T b, T mod) {
    assert(a < mod);
    assert(b < mod);
    if constexpr (std::numeric_limits<T>::min() != 0)
      return (a += b) >= mod ? a - mod : (a < 0 ? a + mod : a);
    else
      return (a += b) >= mod ? a - mod : a;
  }

  static inline T &sadd_mod(T &a, T b, T mod) { return a = add_mod(a, b, mod); }

  static inline T pow_mod(T a, uint64_t b, T mod) {
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

inline uint64_t fast_pow(uint64_t n, uint64_t pow) {
  uint64_t r = 1, p = n;
  while (pow) {
    if (pow & 1)
      r *= p;
    p *= p;
    pow >>= 1;
  }
  return r;
}

inline uint64_t fast_pow(uint64_t n, uint64_t pow, uint64_t mod) {
  uint64_t r = 1, p = n;
  while (pow) {
    if (pow & 1)
      r = r * p % mod;
    p = p * p % mod;
    pow >>= 1;
  }
  return r;
}

template <typename T, typename UP, const T MOD = 1000000007>
struct ModuloOperator : OperatorWithModulo<T, UP> {
  using Base = OperatorWithModulo<T, UP>;
  static inline T mul(T a, T b) { return Base::mul_mod(a, b, MOD); }
  static inline T &smul(T &a, T b) { return Base::smul_mod(a, b, MOD); }
  static inline T add(T a, T b) { return Base::add_mod(a, b, MOD); }
  static inline T &sadd(T &a, T b) { return Base::sadd_mod(a, b, MOD); }
  static inline T pow(T a, T b) { return Base::pow_mod(a, b, MOD); }
};

struct DefaultModuloOperator {
  template <typename T> static inline T mul(T a, T b) { return a * b; }
  template <typename T> static inline T &smul(T &a, T b) { return a *= b; }
  template <typename T> static inline T add(T a, T b) { return a + b; }
  template <typename T> static inline T &sadd(T &a, T b) { return a += b; }
};

template <size_t N, I32 MOD> struct Comb {
  using T = I32;

  Comb() {
    rp(i, N) {
      comb_[i][0] = comb_[i][i] = 1;
      rep(j, 1, i) {
        comb_[i][j] = modulo_.add(comb_[i - 1][j - 1], comb_[i - 1][j]);
      }
    }
  }

  const T *operator[](size_t n) const {
    assert(n < N);
    return comb_[n];
  }

  T comb_[N][N];
  [[no_unique_address]] utils::ModuloOperator<I32, I64, MOD> modulo_;
};

template <size_t N, I32 MOD> struct Factorial {
  using T = I32;

  Factorial() {
    v_[0] = inv_[0] = 1;
    rep(i, 1, N) { v_[i] = modulo_.mul(v_[i - 1], i); }
    inv_[N - 1] = modulo_.pow(v_[N - 1], MOD - 2);
    rev_repd(i, N - 2, 1) { inv_[i] = modulo_.mul(inv_[i + 1], i + 1); }
  }

  T comb(size_t n, size_t k) const {
    assert(k <= n);
    assert(n < N);
    assert(k < N);
    return modulo_.mul(v_[n], modulo_.mul(inv_[k], inv_[n - k]));
  }

  T val(size_t n) const {
    assert(n < N);
    return v_[n];
  }

  T inv(size_t n) const {
    assert(n < N);
    return inv_[n];
  }

  T v_[N], inv_[N];
  [[no_unique_address]] utils::ModuloOperator<I32, I64, MOD> modulo_;
};

template <typename T> struct is_str { static constexpr bool value = false; };
template <> struct is_str<std::string> { static constexpr bool value = true; };
template <> struct is_str<std::string_view> {
  static constexpr bool value = true;
};
template <typename T>
static constexpr bool is_str_v =
    is_str<std::remove_const_t<std::remove_reference_t<T>>>::value;

template <class T, class U> T Cast(U &&x) {
  if constexpr (std::is_same_v<std::remove_const_t<std::remove_reference_t<U>>,
                               T>) {
    return std::forward<U>(x);
  } else if constexpr (is_str_v<U>) {
    T y;
    std::istringstream b(x.data(), x.size());
    b >> y;
    return y;
  } else {
    T y;
    std::ostringstream a;
    a << std::forward<U>(x);
    std::istringstream b(a.str());
    b >> y;
    return y;
  }
}

template <class T>
std::vector<T> Split(std::string_view s, std::string_view x = " ") {
  std::vector<T> res;
  for (;;) {
    if (auto pos = s.find(x); pos == -1) {
      if (!s.empty())
        res.emplace_back(Cast<T>(s));
      return res;
    } else {
      auto e = s.substr(0, pos);
      if (!e.empty()) {
        res.emplace_back(Cast<T>(e));
      }
      s = s.substr(e.size() + x.size());
    }
  }
  return res;
}

static inline void PrintMessage(std::ostream &ostr, std::string_view split) {}
template <typename T, typename... Args>
void PrintMessage(std::ostream &ostr, std::string_view split, T &&arg1,
                  Args &&...rest_args) {
  ostr << arg1;
  if constexpr (sizeof...(rest_args)) {
    ostr << split;
    PrintMessage(ostr, split, std::forward<Args>(rest_args)...);
  }
}
static inline void PrintlnMessage(std::ostream &ostr, std::string_view split) {}
template <typename T, typename... Args>
void PrintlnMessage(std::ostream &ostr, std::string_view split, T &&arg1,
                    Args &&...rest_args) {
  PrintMessage(ostr, split, std::forward<T>(arg1),
               std::forward<Args>(rest_args)...);
  MSG_ENDL(ostr);
}

struct GlobalThreadIDHolder {
  static GlobalThreadIDHolder &instance() {
    static GlobalThreadIDHolder holder;
    return holder;
  }

  uint64_t ThreadID() {
    thread_local uint64_t tid = ++g_tid;
    return tid;
  }
  std::string_view StrThreadID() {
    thread_local std::string stid = std::to_string(ThreadID());
    return stid;
  }

private:
  std::atomic_uint64_t g_tid{};
};

static auto &global_thread_id_holder = GlobalThreadIDHolder::instance();

inline uint64_t get_tid() { return global_thread_id_holder.ThreadID(); }
inline std::string_view get_str_tid() {
  return global_thread_id_holder.StrThreadID();
}

static constexpr size_t kLogTimePointSize = LOG_TIMEPOINT_SIZE;

inline constexpr std::string_view extract_file_name(std::string_view sv) {
  auto &&pos = sv.rfind('/') + 1;
  return {sv.data() + pos, sv.size() - pos};
}

} // namespace utils