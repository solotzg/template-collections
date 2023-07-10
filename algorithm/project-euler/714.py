from py_utils import *


def calc_bit(n):
    s = set()
    c = 0
    while n:
        a = n % 10
        n //= 10
        s.add(a)
        c += 1
    return len(s), c


def calc_last_dig(n):
    s = set()
    for i in range(10):
        a = n * i % 10
        s.add(a)
    return s


def d(n):
    # print("handle", n)
    bs, m = calc_bit(n)
    if bs <= 2:
        return n
    avec = calc_last_dig(n)
    while True:
        res = None
        max_mask = 1 << m
        for a in avec:
            for b in range(10):
                for mask in range(max_mask):
                    x = 0
                    for i in range(m-1, -1, -1):
                        x *= 10
                        if (mask >> i) & 1:
                            x += a
                        else:
                            x += b
                    if (mask >> (m-1)) & 1:
                        if a == 0:
                            continue
                    else:
                        if b == 0:
                            continue
                    if x % n == 0:
                        if res is None:
                            res = x
                        else:
                            res = min(res, x)
        if res is not None:
            return res
        m += 1


def D(n):
    return sum([d(i) for i in range(1, n+1)])


@wrap_run_time
def main():
    assert d(12) == 12
    assert d(102) == 1122
    assert d(103) == 515
    assert d(290) == 11011010
    assert d(317) == 211122
    assert D(110) == 11047
    assert D(500) == 29570988
    a = D(50000)
    b = str(a)
    c = b[1:13]
    c = c if len(c) == 12 else c + '0'*(12-len(c))
    print('{}.{}e{}'.format(b[0], c, len(b)-1))


if __name__ == '__main__':
    main()
