#!/usr/bin/env python3

import math
from py_utils import *


class Solve:
    # result
    #   = n - sum(each_prime(p)_in_range[2,n]{min(p, floor(n/p))})
    #   = n - sum(each_i_in_range(1, sqrt(n)){prime_count_in_range[i, n]})
    #   - n - sum(each_i_in_range(1, sqrt(n)){pi(floor(n/p)) - pi(p-1)})

    def __init__(self, n) -> None:
        self.n = n

    @wrap_run_time
    def run(self):
        self.prime_helper = PrimeHelper(self.n)
        self.prime_helper.init_pi()
        res = self.n
        for p in range(1, 1+int(math.sqrt(self.n))):
            a = self.prime_helper.pi(p-1)
            b = self.prime_helper.pi(self.n // p)
            res -= b-a
        return res

    @wrap_run_time
    def run_optimized(self):
        prime_helper = pybind_utils.gen_prime_helper_with_maxnum(self.n)
        prime_helper.init_pi_small()
        res = self.n
        for p in range(1, 1+int(math.sqrt(self.n))):
            a = prime_helper.pi(p-1)
            b = prime_helper.pi(self.n // p)
            res -= b-a
        return res

    @wrap_run_time
    def bruce_force(self):
        self.prime_helper = PrimeHelper(self.n)
        res = self.n
        for p in range(2, self.n+1):
            if self.prime_helper.is_prime(p):
                res -= min(p, math.floor(self.n/p))

        return res


def test():
    n = 10**8
    assert Solve(n).bruce_force() == Solve(n).run()


def main():
    n = 10**10
    print(Solve(n).run_optimized())


if __name__ == '__main__':
    main()
