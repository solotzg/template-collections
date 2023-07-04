#!/usr/bin/env python3

from py_utils import *


class Solve:
    def __init__(self) -> None:
        self.cache = {}

    def run(self, n):
        max_len = 20
        prime_helper = PrimeHelper.with_max_len(max_len*10)
        primes = prime_helper.primes()
        c = Solve()
        s = 0
        for sz in range(1, 1 + max_len):
            for prime in primes:
                s += c.dp(prime, sz)
                if s >= n:
                    break
            if s >= n:
                break

        num_end = 10**sz - 1
        num_bg = 2

        while num_bg < num_end:
            s = 0
            mid = (num_bg+num_end)//2
            ss = [int(c) for c in str(mid)]
            ss.reverse()
            pp = 0
            lens = len(ss)
            for pos in range(lens - 1, -1, -1):
                for u in range(0, ss[pos]):
                    tp = pp + u
                    for prime in primes:
                        s += c.dp(prime - tp, pos)
                        if s >= n:
                            break
                pp += ss[pos]
                if s >= n:
                    break

            pp = sum(ss)
            if prime_helper.is_prime(pp):
                s += 1

            if s < n:
                num_bg = mid+1
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
        r = 0
        for x in range(0, 10):
            r += self.dp(remain - x, n-1)
        self.cache[(remain, n)] = r
        return r


@wrap_run_time
def main():
    n = 10**16
    print(Solve().run(n))


if __name__ == '__main__':
    main()
