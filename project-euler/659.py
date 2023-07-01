#!/usr/bin/env python3

from functools import reduce
from py_utils import *
from sympy import nthroot_mod, mod_inverse, sqrt_mod
import numpy as np

mod = 10**18


@wrap_run_time
def solve_bruceforce(N):
    res = 0
    tp = []
    for k in range(1, N+1):
        o = 4 * k * k + 1
        p = max(MillerRabin.find_factors(o))
        tp.append(p)

    res = reduce(lambda x, y: (x+y) % mod, tp)
    return res


# [2023/07/01 22:44:42.882 +08:00][DEBUG][`solve`: time cost 52.568s]
# 238518915714422000
@wrap_run_time
def solve(N):
    prime_helper = pybind_utils.gen_prime_helper_with_maxnum(4*N*N+1)
    primes = prime_helper.primes()[1:]
    d = np.zeros(N+1, np.int64)
    c = np.zeros(N+1, np.int64)
    for k in range(1, N+1):
        d[k] = 4 * k * k + 1
    for prime in primes:
        # r = nthroot_mod(-1, 2, prime, True)
        r = sqrt_mod(-1, prime, True)
        inv2 = mod_inverse(2, prime)
        for x in r:
            k_bg = x * inv2 % prime
            for k in range(k_bg, N+1, prime):
                while d[k] % prime == 0:
                    d[k] //= prime
                c[k] = prime
    res = reduce(lambda x, y: (x+y) % mod, (max(x, y)
                 for x, y in zip(c[1:N+1], d[1:N+1])))
    return res


if __name__ == '__main__':
    N = 10**7
    # print(solve_bruceforce(N))
    print(solve(N))
