#!/usr/bin/env python3

from py_utils import *

from math import isqrt


def _A060831(n): return n+sum(n//i for i in range(3, n+1, 2))


def A060831(n):
    m = n // 2
    t = isqrt(m)
    s = isqrt(n)
    return t*t - s*s + (sum(n // k for k in range(1, s+1))-sum(m//k for k in range(1, t+1))) * 2


def f(n, k):
    m = n//k
    return m*(m-1)//2 * k + (n+1-m*k) * m


def f_mod(n, k, mod):
    m = n//k
    return m*(m-1)//2 % mod * k % mod + (n+1-m*k % mod) * m % mod


def g(n):
    r = n*(n+1)//2
    r += sum(sum(i//k for i in range(1, n+1)) for k in range(3, n+1, 2))
    return r


def g3(n):
    r = n*(n+1)//2
    c = 0
    for k in range(3, n+1, 2):
        d = f(n, k)
        c += d
    return r+c


def g4_mod(n, mod):
    r = 0
    m = n//2
    for s in range(1, 1+n):
        s2 = s * s
        if s2 > n:
            break
        bg, ed = s2, (s+1)**2
        ed = min(ed, n+1)
        r += -s2 * (ed-bg) % mod + (f_mod(n, s, mod) -
                                    f_mod(s2-1, s, mod))*2 % mod
        r %= mod

    for t in range(1, 1+n):
        t2 = t * t
        if t2 > m:
            break
        bg, ed = t2*2, 2*(t+1)**2
        ed = min(ed, n+1)
        r += t2 * (ed-bg)
        if n & 1:
            r -= (f_mod(m, t, mod)-f_mod(t2-1, t, mod))*2*2 % mod
        else:
            r -= (f_mod(m-1, t, mod)-f_mod(t2-1, t, mod))*2*2 % mod
            r -= (m//t)*2 % mod
        r %= mod

    return (r+mod) % mod


def g4(n):
    r = 0
    m = n//2
    for t in range(1, 1+n):
        t2 = t * t
        if t2 > m:
            break
        bg, ed = t2*2, 2*(t+1)**2
        ed = min(ed, n+1)
        # print("t={}, bg={}, ed={}".format(t, bg, ed))
        r += t2 * (ed-bg)
        if n & 1:
            r -= (f(m, t)-f(t2-1, t))*2*2
        else:
            r -= (f(m-1, t)-f(t2-1, t))*2*2
            r -= (m//t)*2

    for s in range(1, 1+n):
        s2 = s * s
        if s2 > n:
            break
        bg, ed = s2, (s+1)**2
        ed = min(ed, n+1)
        # print("s={}, bg={}, ed={}".format(s, bg, ed))
        r += -s2 * (ed-bg) + (f(n, s)-f(s2-1, s))*2

    return r


def g2(n):
    return sum(A060831(i) for i in range(1, n+1))


def test():
    for n in range(2, 200):
        assert g(n) == g2(n) == g3(n)
    for n in range(2, 3000):
        assert g3(n) == g4(n)


@wrap_run_time
def main():
    n = 10**16
    print(g4_mod(n, 10**9+7))


if __name__ == '__main__':
    # test()
    main()

# Problem 688
