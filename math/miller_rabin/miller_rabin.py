
#!/usr/bin/python3
import numpy as np

def fast_power(base, power, mod):
    res = 1
    mul = base
    while power:
        if power & 1:
            res = (res * mul) % mod
        mul = (mul * mul) % mod
        power = power >> 1
    return res


def get_random(i, j=None):
    if j is None:
        return np.random.randint(i + 1)
    if i > j:
        i, j = j, i
    return np.random.randint(i, j + 1)


class MillerRabin:
    def __init__(self):
        pass

    @staticmethod
    def is_prime(n, s=10):
        if n == 2:
            return True
        if n & 1 == 0 or n < 2:
            return False
        m, p = n - 1, 0
        while m & 1 == 0:
            m = m >> 1
            p += 1

        for _a in range(s):
            b = fast_power(get_random(2, n - 1), m, n)
            if b == 1 or b == n - 1:
                continue
            for _b in range(p - 1):
                b = fast_power(b, 2, n)
                if b == n - 1:
                    break
            else:
                return False
        return True
