struct PrimeHelper;

void *gen_prime_helper_with_maxlen(uint64_t max_len);
void *gen_prime_helper_with_maxnum(uint64_t max_num);
PrimeHelper *IntoPrimeHelper(void *p);
void prime_helper_init_pi_small(void *p);
uint64_t prime_helper_pi(void *p, uint64_t n);
bool prime_helper_is_prime(void *p, uint64_t n);
void destroy_prime_helper(void *p);
std::unordered_map<uint64_t, uint64_t> prime_helper_decompose(void *p,
                                                              uint64_t num);