#!/usr/bin/env python3

from py_utils import *


class Solve:
    def __init__(self, n) -> None:
        self.n = n
        self.prime_helper = PrimeHelper(int(n**(2.0/3)))
        self.primes = self.prime_helper.primes

    def dfs(self, pre, index):
        res = self.n // pre
        for i in range(index, len(self.primes)):
            p = self.primes[i]
            pw = p ** 3
            if pre * pw > self.n:
                break
            while pre * pw <= self.n:
                res += self.dfs(pre * pw, i+1)
                pw *= p

        return res

    def run(self):
        res = self.dfs(1, 0)
        return res


def s(n):
    r = 1
    for _, c, in prime_helper.decompose(n):
        if c >= 3:
            r *= (c - 2 + 1)
    return r


def S(n):
    global prime_helper
    prime_helper = PrimeHelper(n)
    return sum(s(x) for x in range(1, n+1))


@wrap_run_time
def main():
    print(Solve(10**18).run())


if __name__ == '__main__':
    main()
