#!/usr/bin/env python3

import math
from py_utils import *


class Solve:
    def __init__(self) -> None:
        self.s_cache = {}
        self.s2_cache = {}

    def s2(self, n):
        # f(2n)=1+f(n)
        # f(2n+1)=1+f(n)
        # S2(n) = sum(math.floor(log2(i)) i in [1,n])

        if (n < 2):
            return 0
        r = self.s2_cache.get(n)
        if r is not None:
            return r
        r = n-1 + self.s2(n//2) + self.s2((n-1)//2)
        # real = sum(math.floor(math.log2(i)) for i in range(1, n+1))
        # assert real == r
        self.s2_cache[n] = r
        return r

    def s(self, n):
        # F(2n+1)=F(n)
        # F(2n)=1+math.floor(log2(n))
        if (n < 2):
            return 0
        r = self.s_cache.get(n)
        if r is not None:
            return r
        r = n//2 + self.s((n-1)//2) + self.s2(n//2)
        self.s_cache[n] = r
        return r

    def solve(self, n):
        return self.s(n)


@wrap_run_time
def main():
    s = Solve()
    fmt = 'S({})={}'
    print(fmt.format(100, s.solve(100)))
    print(fmt.format(10**7, s.solve(10**7)))
    print(fmt.format(10**16, s.solve(10**16)))


if __name__ == '__main__':
    main()
