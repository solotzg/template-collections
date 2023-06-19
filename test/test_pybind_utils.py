from functools import reduce
import unittest
from py_utils import *


class TestPyBindUtils(unittest.TestCase):

    def test_prime_helper(self):
        n = 1000000
        prime_helper = pybind_utils.gen_prime_helper_with_maxnum(n)
        pybind_utils.prime_helper_init_pi_small(prime_helper)
        a: dict = pybind_utils.prime_helper_decompose(prime_helper, n)
        assert reduce(lambda x, y: x*y, [k**v for k, v in a.items()]) == n
        pybind_utils.destroy_prime_helper(prime_helper)


if __name__ == '__main__':
    unittest.main()
