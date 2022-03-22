#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>


struct MillerRabin
{
public:
    void FindFacs(int64_t n)
    {
        this->fnum_ = 0;
        FindFacsImpl(n);
    }
    const static int Times = 20;

    // miller_rabin, return `true` if prime
    // max wrong rate: 1 / (4 ^ times)
    bool IsPrime(int64_t n)
    {
        if (n == 2)
            return 1;
        if (n < 2 || !(n & 1))
            return 0;
        int64_t x = n - 1;
        int64_t t = 0;
        while ((x & 1) == 0)
        {
            x >>= 1;
            t++;
        }
        for (int i = 0; i < Times; i++)
        {
            int64_t a = random() % (n - 1) + 1;
            if (is_composite(a, n, x, t))
                return 0; //合数
        }
        return 1;
    }

    size_t fnum() const { return fnum_; }
    const int64_t * factors() const { return factors_; }

private:
    int64_t add(int64_t a, int64_t b, int64_t mod)
    {
        return a += b, a >= mod ? a - mod : a;
    }

    int64_t multi(int64_t a, int64_t b, int64_t mod)
    {
        int64_t res = 0;
        while (b)
        {
            if (b & 1)
                res = add(res, a, mod);
            a = add(a, a, mod);
            b >>= 1;
        }
        return res;
    }

    int64_t pow(int64_t a, int64_t b, int64_t mod)
    {
        int64_t res = 1;
        while (b)
        {
            if (b & 1)
                res = multi(res, a, mod);
            a = multi(a, a, mod);
            b >>= 1;
        }
        return res;
    }

    // miller_rabin, return `true` if composite
    bool is_composite(int64_t a, int64_t n, int64_t x, int64_t t)
    {
        int64_t ret = pow(a, x, n);
        int64_t last = ret;
        for (int i = 1; i <= t; i++)
        {
            ret = multi(ret, ret, n);
            if (ret == 1 && last != 1 && last != n - 1)
                return 1;
            last = ret;
        }
        if (ret != 1)
            return 1;
        return 0;
    }


    int64_t gcd(int64_t a, int64_t b)
    {
        if (a == 0)
            return 1;
        if (a < 0)
            return gcd(-a, b);
        while (b)
        {
            int64_t t = a % b;
            a = b;
            b = t;
        }
        return a;
    }

    int64_t pollard_rho(int64_t x, int64_t c)
    {
        int64_t i = 1, k = 2;
        int64_t x0 = random() % x;
        int64_t y = x0;
        while (1)
        {
            i++;
            x0 = (multi(x0, x0, x) + c) % x;
            int64_t d = gcd(y - x0, x);
            if (d != 1 && d != x)
                return d;
            if (y == x0)
                return x;
            if (i == k)
            {
                y = x0;
                k += k;
            }
        }
    }

    void FindFacsImpl(int64_t n)
    {
        if (IsPrime(n))
        {
            factors_[fnum_++] = n;
            return;
        }
        int64_t p = n;
        while (p >= n)
            p = pollard_rho(p, random() % (n - 1) + 1);
        FindFacsImpl(p);
        FindFacsImpl(n / p);
    }

    // pollard_rho
    int64_t factors_[100]; // factorization no order
    int fnum_; // num of prime factor
};

#define MSGLN(s)                     \
    do                               \
    {                                \
        std::cout << s << std::endl; \
    } while (false)


int main()
{
    MillerRabin m;
    int t;
    int64_t n, res;
    std::cin >> t;
    MSGLN("Start to check " << t << " numbers");
    const auto is_prime = [](int64_t num) {
        int64_t mid = std::sqrt(num);
        for (int64_t x = 2; x <= mid; ++x)
        {
            if (num % x == 0)
                return 0;
        }
        return 1;
    };
    while (t--)
    {
        auto num = random() % int64_t(1e9) + 1;
        MSGLN("check " << num);
        if (m.IsPrime(num))
        {
            assert(is_prime(num));
            MSGLN("Prime");
        }
        else
        {
            assert(!is_prime(num));
            MSGLN("Not Prime");
        }
    }
    return 0;
}
