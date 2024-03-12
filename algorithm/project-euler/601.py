#!/usr/bin/env python3

from py_utils import *
import math


def streak(n):
    k = 1
    while True:
        if (n+k) % (k+1) != 0:
            return k
        k += 1

# 2 * a - 1 -> 2 * t + 1
# 2 * a - 1 = 3 * b - 2 -> 6 * t + 1
# 2 * a - 1 = 3 * b - 2 = 4 * c - 3 -> 12 * t + 1
# f(k) = lsm(2 ~ k-1) * t + 1
    # match GE than k
# f(k+1) = lsm(2 ~ k) * t + 1
    # match GE than k+1
# f(k) exclude f(k+1) match k


def P(s, n):
    u = 1
    for k in range(1, s):
        k += 1
        u = math.lcm(u, k)
    x = (n-1)//u if u != 1 else n-2
    y = (n-1) // math.lcm(u, s+1)
    ans = x - y
    return ans


@wrap_run_time
def main():
    assert (streak(13) == 4)
    assert (streak(120) == 1)
    assert (P(3, 14) == 1)
    assert (P(6, 10**6) == 14286)
    print(sum(P(i, 4**i) for i in range(1, 31+1)))


if __name__ == '__main__':
    main()
