#include <utils/utils.h>

using U8 = uint8_t;
using VU8 = std::vector<uint8_t>;
constexpr ULL MOD = 1e9 + 7;

struct Unit : VU8 {
  using VU8::VU8;

  Unit operator*(const Unit &tar) {
    if (empty())
      return tar;
    if (tar.empty())
      return *this;
    Unit res(size() + tar.size() - 1);
    rp(i, size()) rp(j, tar.size()) { res[i + j] ^= (*this)[i] & tar[j]; }
    return res;
  }

  Unit(VU8 &&src) : VU8(std::forward<VU8>(src)) {}

  Unit pow(ULL n) {
    Unit res, p = (*this);
    for (; n;) {
      if (n & 1)
        res = res * p;
      p = p * p;
      n >>= 1;
    }
    return res;
  }
};

inline uint64_t fast_pow(uint64_t n, U128 pow, uint64_t mod) {
  uint64_t r = 1, p = n;
  while (pow) {
    if (pow & 1)
      r = r * p % mod;
    p = p * p % mod;
    pow >>= 1;
  }
  return r;
}

// https://math.stackexchange.com/questions/4628358/xor-product-modulo-prime
int main() {
  SHOW_TIME_COST();
  Unit n11(VU8{1, 1, 0, 1});
  auto x = n11.pow(utils::fast_pow(3, 8));
  U128 n = 1ull << 52;
  ULL res = 0;
  rev_rep(i, x.size() - 1, -1) {
    if (!x[i])
      continue;
    auto y = fast_pow(2, (n * i), MOD);
    res = (res + y) % MOD;
  }
  MSGLN(res);
  return 0;
}