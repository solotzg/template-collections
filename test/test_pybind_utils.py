from functools import reduce
import unittest
from py_utils import *


class TestPyBindUtils(unittest.TestCase):

    def test_prime_helper(self):
        n = 600000
        prime_helper = pybind_utils.gen_prime_helper_with_maxnum(n)
        prime_helper.init_pi_small()
        a: list = prime_helper.decompose(n)
        assert reduce(lambda x, y: x*y, (k**v for k, v in a)) == n
        y = sum(1 for i in range(1, n+1) if prime_helper.is_prime(i))
        x = prime_helper.pi(n)
        assert y == x
        primes = prime_helper.primes()

        prime_helper2 = PrimeHelper.with_max_len(130)
        assert prime_helper2.primes == primes[:31]

    @unittest.skip
    def test_miller_rabin_big(self):
        n = 2050630921381605153620210217162793974942622874069
        assert not MillerRabin.is_prime(n)
        f = MillerRabin.find_factors(n)
        assert reduce(lambda x, y: x*y, f) == n
        assert MillerRabin.is_prime(87678262537778991782390312341223827409)

    def test_miller_rabin_small(self):
        n = 147830009 * 69535001581
        assert not MillerRabin.is_prime(n)
        f = MillerRabin.find_factors(n)
        assert reduce(lambda x, y: x*y, f) == n
        assert MillerRabin.is_prime(87678262537778991782390312341223827409)


if __name__ == '__main__':
    unittest.main()
