import math
import array


class PrimeHelper:
    @staticmethod
    def with_max_len(max_len):
        return PrimeHelper(max_len**2)

    def __init__(self, maxn):
        self._maxn = maxn
        self._max_size = math.floor(math.sqrt(self._maxn)) + 1
        try:
            import numpy
            self._is_prime = numpy.ones(self._max_size, dtype=numpy.uint8)
        except:
            self._is_prime = array.array('b',  [True] * self._max_size)
        self._primes = []
        self._calc_primes()

    def init_pi(self):
        self._pi_data = array.array('q',  [0] * self._max_size)
        prime_cnt = 0
        for i in range(2, self._max_size):
            if self._is_prime[i]:
                prime_cnt += 1
            self._pi_data[i] = prime_cnt

    @property
    def primes(self):
        return self._primes

    def is_prime(self, n):
        assert n <= self._maxn
        if n < self._max_size:
            return self._is_prime[n]

        if 0 == (n & 1):
            return False

        for p in self.primes:
            if p * p > n:
                break
            if n % p == 0:
                return False
        return True

    def _calc_primes(self):
        self._is_prime[0] = self._is_prime[1] = False
        for i in range(2, self._max_size):
            if self._is_prime[i]:
                self._primes.append(i)
            for p in self._primes:
                t = p * i
                if t >= self._max_size:
                    break
                self._is_prime[t] = False
                if i % p == 0:
                    break

    def pi(self, n):
        if n < self._max_size:
            return self._pi_data[n]
        r = icbrt(n)
        k = int(math.sqrt(n))
        pr = self.pi(r)
        pk = self.pi(k)
        mu = pk - pr
        s = self._foo(pr - 1, n) - 1 + pr - mu * (mu + 1) // 2 + mu * pk
        for i in range(pr, pk):
            s -= self.pi(n // self._primes[i])
        return s

    def _foo(self, i, n):
        if i < 0:
            return n
        if n == 0:
            return 0
        if self._primes[i] > n:
            return 1
        s = n
        while i >= 0:
            s -= self._foo(i - 1, n // self._primes[i])
            i -= 1
        return s

    def decompose(self, n):
        res = []
        for p in self.primes:
            if p * p > n:
                break
            if n % p == 0:
                c = 0
                while n % p == 0:
                    c += 1
                    n //= p
                res.append((p, c))
        if n != 1:
            res.append((n, 1))
        return res


def icbrt(n):
    s = int(n ** (1.0 / 3))
    while (s + 1) * (s + 1) * (s + 1) <= n:
        s += 1
    while s * s * s > n:
        s -= 1
    return s
