#!/usr/bin/env python3

from py_utils import *
import sympy

# g(n) = 3n if gcd(n, g(n-1)) != 1
# g(n + i) = 3n + i if gcd(n+i, 3n+i-1) == 1 (1 <= i < k)
# gcd(n+i, 3n+i-1) = gcd(n+i, 2n-1)
# 2n-1 is odd => gcd(n+i, 2n-1) = gcd(2n+2i, 2n-1) = gcd(2i+1, 2n-1)
# k = (min(primefactors(2n-1))-1)/2


def f(m):
    n = 9
    g = 27
    while True:
        s = sympy.primefactors(2*n-1)
        k = (s[0]-1)//2
        next_n = n + k
        if m < next_n:
            return m - n + g
        n += k
        g = n * 3
        # print("g({})={}".format(n, g))


def main():
    # assert f(1000) == 2524
    # assert f(10**6) == 2624152
    print(f(10**15))
    # print(p)


if __name__ == '__main__':
    main()
