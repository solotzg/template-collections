
#!/usr/bin/python3

import random
import math
from functools import reduce
import time


def wrap_run_time(func):
    def wrap_func(*args, **kwargs):
        bg = time.time()
        r = func(*args, **kwargs)
        print('Time cost {:.3f}s'.format(time.time() - bg))
        return r

    return wrap_func


class MillerRabin:

    @staticmethod
    def miller_rabin_test(n, d, p):
        x = pow(random.randint(2, n - 1), d, n)
        if x == 1 or x == n - 1:
            return True
        for _ in range(p):
            x = x * x % n
            if x == 1:
                return False
            if x == n - 1:
                return True
        return False

    @staticmethod
    def is_prime(n, s=20):
        if n == 2:
            return True
        if n & 1 == 0 or n < 2:
            return False
        d, p = n - 1, 0
        while d & 1 == 0:
            d = d >> 1
            p += 1

        for _ in range(s):
            if not MillerRabin.miller_rabin_test(n, d, p):
                return False
        return True

    @staticmethod
    def pollard_rho_algorithm(x, c):
        a, k, x0 = 1, 2, random.randint(0, x-1)
        y = x0
        while True:
            a += 1
            x0 = (x0 * x0 + c) % x
            d = math.gcd((y-x0), x)
            if d != 1 and d != x:
                return d
            if y == x0:
                return x
            if a == k:
                y = x0
                k = k << 1

    @staticmethod
    def find_factors_impl(n, s, res: list):
        if MillerRabin.is_prime(n, s):
            res.append(n)
            return
        p = n
        while p >= n:
            p = MillerRabin.pollard_rho_algorithm(p, random.randint(2, n-1))
        MillerRabin.find_factors_impl(p, s, res)
        MillerRabin.find_factors_impl(n//p, s, res)

    @staticmethod
    def find_factors(n, s=20):
        res = []
        MillerRabin.find_factors_impl(n, s, res)
        return res


def test_miller_rabin():

    @wrap_run_time
    def test1():
        n = 2050630921381605153620210217162793974942622874069
        assert not MillerRabin.is_prime(n)
        f = MillerRabin.find_factors(n)
        print("factors of {} is {}".format(n, f))
        assert reduce(lambda x, y: x*y, f) == n

    test1()

    assert MillerRabin.is_prime(87678262537778991782390312341223827409)


if __name__ == '__main__':
    test_miller_rabin()
