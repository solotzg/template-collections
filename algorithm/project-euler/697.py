#!/usr/bin/env python3

import math
from py_utils import *


@wrap_run_time
def solve_bsearch(n, z):
    import scipy.special as sc

    bg = (-20000000)
    ed = math.log(z)
    for _ in range(100):
        lnp = (bg + ed) / 2
        if sc.gammaincc(n, -lnp) > z:
            ed = lnp
        else:
            bg = lnp

    return (-bg)/math.log(10)


@wrap_run_time
def solve2(n, z):
    from scipy.stats import gamma

    x = gamma(n).ppf(1-z)/math.log(10)
    return x


def main():
    r = "{:.2f}".format(solve_bsearch(10**7, 0.25))
    r2 = "{:.2f}".format(solve2(10**7, 0.25))
    assert r == r2
    print(r)


if __name__ == '__main__':
    main()
