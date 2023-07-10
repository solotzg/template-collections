#!/usr/bin/env python3

from py_utils import *


class Solve:
    def __init__(self) -> None:
        self.cache = {}

    def find_max_len(self, n):
        max_len = 20
        s = 0
        self.prime_helper = PrimeHelper.with_max_len(max_len * 10)
        self.primes = self.prime_helper.primes()

        for sz in range(1, 1 + max_len):
            for prime in self.primes:
                s += self.dp(prime, sz)
                if s >= n:
                    return sz
        assert False

    def run(self, n):
        max_len = self.find_max_len(n)
        num_end = 10 ** max_len - 1
        num_bg = 2

        while num_bg < num_end:
            mid = (num_bg+num_end)//2
            ss = [int(c) for c in reversed(str(mid))]
            s = 1 if self.prime_helper.is_prime(sum(ss)) else 0
            pre_sum = 0
            for pos in range(len(ss) - 1, -1, -1):
                for u in range(0, ss[pos]):
                    for prime in self.primes:
                        s += self.dp(prime - (pre_sum + u), pos)
                        if s >= n:
                            break
                if s >= n:
                    break
                pre_sum += ss[pos]
            if s < n:
                num_bg = mid + 1
            else:
                num_end = mid
        return num_bg

    def dp(self, remain, n):
        if remain < 0:
            return 0
        r = self.cache.get((remain, n))
        if r is not None:
            return r
        if remain == 0:
            return 1
        if n == 0:
            return 0
        r = sum(self.dp(remain - x, n-1) for x in range(0, 10))
        self.cache[(remain, n)] = r
        return r


@wrap_run_time
def main():
    n = 10**16
    res = Solve().run(n)
    assert res == 45009328011709400


if __name__ == '__main__':
    main()
