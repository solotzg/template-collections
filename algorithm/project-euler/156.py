#!/usr/bin/env python3

from py_utils import *


class Pe156:
    def __init__(self) -> None:
        self._cache2 = {}

    def _dfs2(self, n, d):
        if n == 0:
            return 0
        if n == 1:
            return 1
        x = self._cache2.get((n, d))
        if x is not None:
            return x
        x = 9 * self._dfs2(n-1, d) + self._dfs1(1, n-1, d)
        self._cache2[(n, d)] = x
        return x

    def _dfs1(self, pre_cnt, n, d):
        return pre_cnt * (10**n) + self._dfs2(n, d)

    def cnt_d_maxn(self, maxn, d):
        def deccode(x):
            r = []
            while x:
                r.append(x % 10)
                x //= 10
            r.reverse()
            return r
        x = deccode(maxn)
        n = len(x)
        pre = 0
        ans = 0
        for i, c in enumerate(x):
            if d <= c-1:
                ans += self._dfs1(pre+1, n-i-1, d)
                ans += (c-1) * self._dfs1(pre, n-i-1, d)
            else:
                ans += (c) * self._dfs1(pre, n-i-1, d)
            if c == d:
                pre += 1
        ans += self._dfs1(pre, 0, d)
        return ans

    def _bs(self, d, bg, ed, bg_v, ed_v):
        if bg+1 == ed:
            return bg if bg_v == bg else 0

        ans = 0
        mid = (bg+ed)//2
        mid_v = self.cnt_d_maxn(mid, d)

        if mid > bg and mid_v >= bg and bg_v <= mid:
            ans += self._bs(d, bg, mid, bg_v, mid_v)

        if ed > mid and ed_v >= mid and mid_v <= ed:
            ans += self._bs(d, mid, ed, mid_v, ed_v)
        return ans

    def solve(self, d, bg, ed):
        return self._bs(d, bg, ed, self.cnt_d_maxn(bg, d), self.cnt_d_maxn(ed, d))


@wrap_run_time
def main():
    s = Pe156()
    assert 5 == s.cnt_d_maxn(12, 1)
    assert 4 == s.cnt_d_maxn(11, 1)
    assert 2 == s.cnt_d_maxn(10, 1)
    assert 199981 == s.cnt_d_maxn(199981, 1)

    assert sum(s.solve(d, 1, 10**20) for d in range(1, 9+1)) == 21295121502550


if __name__ == '__main__':
    main()
