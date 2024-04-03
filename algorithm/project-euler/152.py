#!/usr/bin/env python3

import math
from py_utils import *


class N:
    def __init__(self, a, b) -> None:
        self.a = a
        self.b = b

    def cmp(self, t):
        return self._minus(t)

    def _minus(self, t):
        return self.a * t.b - self.b * t.a

    def _add(self, t):
        return self.a * t.b + self.b * t.a

    def add(self, t):
        a = self._add(t)
        b = self.b * t.b
        gcd = math.gcd(a, b)
        a //= gcd
        b //= gcd
        return N(a, b)

    def minus(self, t):
        a = self._minus(t)
        b = self.b * t.b
        gcd = math.gcd(a, b)
        a //= gcd
        b //= gcd
        return N(a, b)

    def __str__(self) -> str:
        return "{}/{}".format(self.a, self.b)


def check(p, maxn):
    x = []
    c = p
    while c <= maxn:
        x.append(c)
        c += p
    n = len(x)
    for mask in range(1, (1 << n)):
        k = N(0, 1)
        for i in range(n):
            if (mask >> i) & 1:
                k = k.add(N(1, x[i]**2))
        if k.b % p != 0:
            return True
    return False


def check2(p, z):
    ans = set()
    x = []
    for c in z:
        if c % p == 0:
            x.append(c)
    n = len(x)
    for mask in range(1, (1 << n)):
        k = N(0, 1)
        for i in range(n):
            if (mask >> i) & 1:
                k = k.add(N(1, x[i]**2))
        if k.b % p != 0:
            for i in range(n):
                if (mask >> i) & 1:
                    ans.add(x[i])
    return ans


def lower_bound(d, tar):
    bg, ed = 0, len(d)-1
    while bg < ed:
        mid = (bg+ed)//2
        if d[mid] >= tar:
            ed = mid
        else:
            bg = mid+1
    if d[bg] >= tar:
        return bg
    return bg+1


def upper_bound(d, tar):
    bg, ed = 0, len(d)-1
    while bg < ed:
        mid = (bg+ed)//2
        if d[mid] > tar:
            ed = mid
        else:
            bg = mid+1
    if d[bg] > tar:
        return bg
    return bg+1


@wrap_run_time
def main():
    maxn = 80
    prime_helper = pybind_utils.gen_prime_helper_with_maxnum(maxn)

    s = [n for n in range(2, maxn//2+1) if prime_helper.is_prime(n)]
    p = [a for a in s if check(a, maxn)]
    assert p == [2, 3, 5, 7, 13]
    # print(p)
    z = [a for a in range(
        2, maxn+1) if not any(True for k, _ in prime_helper.decompose(a) if k not in p)]
    assert z == [2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 18, 20, 21, 24, 25, 26, 27, 28, 30,
                 32, 35, 36, 39, 40, 42, 45, 48, 49, 50, 52, 54, 56, 60, 63, 64, 65, 70, 72, 75, 78, 80]
    z = set(z)
    for a in (5, 7, 13):
        o = check2(a, z)
        to_remove = []
        for y in z:
            if y % a == 0:
                if y not in o:
                    to_remove.append(y)
        for k in to_remove:
            z.remove(k)
    # print(z, len(z))
    assert z == {2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 18, 20, 21, 24, 27,
                 28, 30, 32, 35, 36, 39, 40, 42, 45, 48, 52, 54, 56, 60, 63, 64, 70, 72, 80}

    lcm = 1
    for x in z:
        lcm = math.lcm(lcm, x)
    lcm2 = lcm**2
    z2 = [lcm2//(x*x) for x in z]
    f_1_2 = lcm2 // 2

    m = len(z)//2
    rem = len(z) - m
    cach1 = []
    cach2 = []

    m_mask = 1 << m
    for mask in range(0, m_mask):
        c = 0
        for i in range(m):
            if mask & (1 << i):
                if c > f_1_2:
                    break
                c = c + z2[i]
        if c <= f_1_2:
            cach1.append(c)

    rem_mask = 1 << rem
    for mask in range(0, rem_mask):
        c = 0
        for i in range(rem):
            if mask & (1 << i):
                if c > f_1_2:
                    break
                c = c + z2[i + m]
        if c <= f_1_2:
            cach2.append(c)

    # print(len(cach1), len(cach2))

    cach1.sort()
    cach2.sort()

    ans = 0
    for a in cach1:
        if a > f_1_2:
            break
        expect = f_1_2 - a
        p1 = lower_bound(cach2, expect)
        if p1 == len(cach2):
            continue
        p2 = upper_bound(cach2, expect)
        ans += p2-p1
    print(ans)


if __name__ == '__main__':
    main()
