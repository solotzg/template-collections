import math


class PrimeHelper:
    def __init__(self, maxn):
        self._maxn = maxn
        self._max_size = math.floor(math.sqrt(self._maxn)) + 1
        try:
            import numpy
            self._is_prime = numpy.ones(self._max_size, dtype=numpy.uint8)
        except:
            self._is_prime = [True] * self._max_size
        self._primes = []
        self.calc_primes()

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

    def calc_primes(self):
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
