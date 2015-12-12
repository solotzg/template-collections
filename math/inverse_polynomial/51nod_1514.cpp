#include <cstdio>
#include <complex>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
struct PolynomialInverse {
    typedef long long ll;
    typedef vector<ll> VL;
    ll mod_v;
    VL b,c,eps,inv_eps;
    int tot;
    long long power(long long x, long long p) {
        long long v = 1;
        while(p) {
            if(p & 1) v = x * v % mod_v;
            x = x * x % mod_v;
            p >>= 1;
        }
        return v;
    }

    void init_eps(int n) {
        tot = n;
        long long base = power(3, (mod_v - 1) / n);
        long long inv_base = power(base, mod_v - 2);
        eps[0] = 1, inv_eps[0] = 1;
        for(int i = 1; i < n; ++i) {
            eps[i] = eps[i - 1] * base % mod_v;
            inv_eps[i] = inv_eps[i - 1] * inv_base % mod_v;
        }
    }

    long long inc(long long x, long long d) {
        x += d;
        return x >= mod_v ? x - mod_v : x;
    }

    long long dec(long long x, long long d) {
        x -= d;
        return x < 0 ? x + mod_v : x;
    }

    void transform(int n, VL & x, VL & w) {
        for(int i = 0, j = 0; i != n; ++i) {
            if(i > j) std::swap(x[i], x[j]);
            for(int l = n >> 1; (j ^= l) < l; l >>= 1);
        }

        for(int i = 2; i <= n; i <<= 1) {
            int m = i >> 1;
            for(int j = 0; j < n; j += i) {
                for(int k = 0; k != m; ++k) {
                    long long z = x[j + m + k] * w[tot / i * k] % mod_v;
                    x[j + m + k] = dec(x[j + k], z);
                    x[j + k] = inc(x[j + k], z);
                }
            }
        }
    }

    void polynomial_inverse(int deg, VL & a, VL & b, VL & tmp) {
        if(deg == 1) {
            b[0] = power(a[0], mod_v - 2);
        } else {
            polynomial_inverse((deg + 1) >> 1, a, b, tmp);

            int p = 1;
            while(p < deg << 1) p <<= 1;
            copy(a.begin(), a.begin() + deg, tmp.begin());
            fill(tmp.begin() + deg, tmp.begin() + p, 0);
            transform(p, tmp, eps);
            transform(p, b, eps);
            for(int i = 0; i != p; ++i) {
                b[i] = (2 - tmp[i] * b[i] % mod_v) * b[i] % mod_v;
                if(b[i] < 0) b[i] += mod_v;
            }
            transform(p, b, inv_eps);
            long long inv = power(p, mod_v - 2);
            for(int i = 0; i != p; ++i)
                b[i] = b[i] * inv % mod_v;
            fill(b.begin() + deg, b.begin() + p, 0);
        }
    }
    PolynomialInverse(int n, ll mod, VL & a) {
        int m = 1;
        for (;m < (n+1)<<1; m<<=1);
        b.assign(m+5, 0);
        c = eps = inv_eps = b;
        this->mod_v = mod;
        init_eps(m);
        polynomial_inverse(n+1, a, b, c);
    }
};
int main() {
    int n = 100000;
    long long mod_v = 998244353;
    vector<long long> a(n+5);
    a[0] = 1;
    for (int i = 1; i<=n; ++i) a[i] = a[i-1]*i%mod_v;
    PolynomialInverse *_p = new PolynomialInverse(n, mod_v, a);
    vector<long long> & b = _p->b;
    for(int i = 0; i <= n; ++i)
        b[i] = (-b[i] % mod_v + mod_v) % mod_v;
    b[0] = (b[0]+1)%mod_v;
    int t, d;
    scanf("%d", &t);
    while(t--) {
        scanf("%d", &d);
        printf("%lld\n", b[d]);
    }
    return 0;
}
