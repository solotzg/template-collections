#pragma once

#include "utils/work.h"
#include <array>
#include <cassert>
#include <cstdint>
#include <exception>
#include <limits>
#include <sstream>

template <size_t N, int M = 5, typename T = int64_t, typename UP = __int128_t>
struct BigInt {
  using Data = std::array<T, N>;
  static constexpr size_t BitCnt = (sizeof(T) * 8 - M);
  static constexpr T MAX_NUM = T(1) << BitCnt;
  static constexpr T MAX_NUM_MASK = MAX_NUM - 1;

  BigInt operator*(const BigInt &x) const {
    BigInt res{};
    std::array<UP, N> tmp{};
    for (size_t i = 0; i < size(); ++i) {
      for (size_t j = 0; j < x.size(); ++j) {
        tmp[i + j] += UP(data_[i]) * x[j];
      }
    }
    for (size_t i = 0; i < x.size() + size(); ++i) {
      res[i] = tmp[i] % MAX_NUM;
      tmp[i + 1] += tmp[i] / MAX_NUM;
      if (res[i] != 0) {
        res.size_ = i + 1;
      }
    }
    return res;
  }

  BigInt operator+(const BigInt &x) const {
    BigInt res{};
    res += x;
    return res;
  }

  BigInt operator-(const BigInt &x) const {
    BigInt res{};
    res -= x;
    return res;
  }

  BigInt operator/(const BigInt &x) const {
    auto r = cmp(x);
    if (r == 0)
      return 1;
    else if (r < 0)
      return 0;

    BigInt res = 1;
    auto tar = *this;

    while (tar) {
      auto p = x;
      for (;;) {
        auto r = tar.cmp(p);
        if (r == 0) {
          break;
        }
      }
    }
  }

  BigInt &operator-=(const BigInt &x) { return InnerOp<false>(x); }

  BigInt &operator+=(const BigInt &x) { return InnerOp<true>(x); }

  BigInt &operator*=(const BigInt &x) { return *this = (*this) * x; }

  BigInt &operator<<=(int c) {
    for (int i = 0; i < c; ++i) {
      ShiftLeft();
    }
    return *this;
  }

  BigInt operator<<(int c) {
    BigInt res = *this;
    res <<= c;
    return res;
  }

  std::string Show() const {
    std::stringstream ss;
    ss << (flag ? '+' : '-');
    ss << "base_" << MAX_NUM;
    ss << "(";
    for (size_t i = 0; i < size_; ++i) {
      if (i != 0)
        ss << ",";
      ss << data_[i];
    }
    ss << ")";
    return ss.str();
  }

  BigInt(T x = 0) {
    static_assert(UP(MAX_NUM_MASK) * N <= std::numeric_limits<T>::max());
    if (x < 0)
      x = -x, flag = false;
    while (x) {
      data_[size_++] = x % MAX_NUM;
      x /= MAX_NUM;
    }
  }

  bool operator==(const BigInt &x) const { return cmp(x) == 0; }

  int cmp(const BigInt &x) const {
    if (auto r = int(size()) - int(x.size()); r) {
      return r > 0 ? 1 : -1;
    }
    for (size_t i = 0; i < size(); ++i) {
      if (auto r = data_[i] - x[i]; r) {
        return r > 0 ? 1 : -1;
      }
    }
    return 0;
  }

  size_t size() const { return size_; }

private:
  T &operator[](size_t idx) { return data_[idx]; }
  const T &operator[](size_t idx) const { return data_[idx]; }

  void ShiftLeft() {
    T pre = 0;
    for (size_t i = 0; i < size(); ++i) {
      data_[i] <<= 1;
      data_[i] |= pre;
      pre = (data_[i] & MAX_NUM) >> BitCnt;
      data_[i] &= MAX_NUM_MASK;
    }
    if (pre) {
      data_[size_++] = 1;
    }
  }

  void ShiftRight() {
    T pre = 0;
    for (size_t i = 0; i < size(); ++i) {
      data_[i] <<= 1;
      data_[i] |= pre;
      pre = (data_[i] & MAX_NUM) >> BitCnt;
      data_[i] &= MAX_NUM_MASK;
    }
    if (pre) {
      data_[size_++] = 1;
    }
  }

  template <bool is_add = true> BigInt &InnerOp(const BigInt &x) {
    const auto *pa = this;
    const auto *pb = &x;
    int order = 1;
    if (pa->size() > pb->size()) {
      std::swap(pa, pb);
      order = -1;
    }
    auto &a = *pa;
    auto &b = *pb;
    T pre = 0;
    for (size_t i = 0; i < a.size(); ++i) {
      if constexpr (is_add)
        data_[i] = a[i] + b[i] + pre;
      else
        data_[i] = (a[i] - b[i]) * order + pre;
      pre = data_[i] / MAX_NUM;
      data_[i] %= MAX_NUM;
    }

    for (size_t i = a.size(); i < b.size(); ++i) {
      if constexpr (is_add)
        data_[i] = b[i] + pre;
      else
        data_[i] = (-b[i]) * order - pre;
      pre = data_[i] / MAX_NUM;
      data_[i] %= MAX_NUM;
    }
    size_ = b.size();
    while (pre) {
      data_[size_++] = pre % MAX_NUM;
      pre /= MAX_NUM;
    }
    while (size_ && data_[size_ - 1] == 0) {
      size_--;
    }
    return *this;
  }

private:
  bool flag = true;
  size_t size_{};
  Data data_;
};

DECLARE_DEBUG_TEST_CODE(static void _test_big_int() {
  using BINT = BigInt<5000, 15>;
  {
    BINT big_int = -BINT::MAX_NUM / 2;

    MSGLN(big_int.Show());
    big_int *= -BINT::MAX_NUM / 2;
    MSGLN(big_int.Show());
    big_int += big_int;
    MSGLN(big_int.Show());
    big_int += big_int;
    MSGLN(big_int.Show());
    big_int *= BINT::MAX_NUM / 2;
    MSGLN(big_int.Show());
    big_int *= BINT::MAX_NUM / 2;
    MSGLN(big_int.Show());
    big_int -= big_int;
    MSGLN(big_int.Show());
    big_int -= BINT::MAX_NUM;
    MSGLN(big_int.Show());
    big_int -= BINT::MAX_NUM / 2;
    MSGLN(big_int.Show());
  }
  {
    BINT big_int = BINT::MAX_NUM / 2 + 1;
    big_int <<= 1;
    MSGLN(big_int.Show());
  }
})