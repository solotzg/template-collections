#include "math/Template/prime_factors.hpp"

void *gen_prime_helper_with_maxlen(uint64_t max_len) {
  return PrimeHelper::GenPrimeHelperPtrWithMaxLen(max_len);
}
void *gen_prime_helper_with_maxnum(uint64_t max_num) {
  return PrimeHelper::GenPrimeHelperPtrWithMaxNum(max_num);
}
PrimeHelper *IntoPrimeHelper(void *p) {
  return reinterpret_cast<PrimeHelper *>(p);
}
void prime_helper_init_pi_small(void *p) {
  return IntoPrimeHelper(p)->InitPiSmall();
}
uint64_t prime_helper_pi(void *p, uint64_t n) {
  return IntoPrimeHelper(p)->Pi(n);
}
bool prime_helper_is_prime(void *p, uint64_t n) {
  return IntoPrimeHelper(p)->IsPrime(n);
}
void destroy_prime_helper(void *p) { delete IntoPrimeHelper(p); }
std::unordered_map<uint64_t, uint64_t> prime_helper_decompose(void *p,
                                                              uint64_t num) {
  std::unordered_map<uint64_t, uint64_t> res;
  IntoPrimeHelper(p)->Decompose(
      num, [&](uint64_t f, size_t cnt) { res.emplace(f, cnt); });
  return res;
}
