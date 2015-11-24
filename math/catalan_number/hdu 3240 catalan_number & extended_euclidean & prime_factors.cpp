#include <cstdio>
#include <complex>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
using namespace std;
typedef long long ll;
struct Prime {
    int _maxn, _sn;
    vector<int> _vis;
    vector<int> _prime;
    Prime(int maxn):_maxn(maxn) {
        _sn = (int) sqrt(_maxn) + 5;
        _vis.assign(_sn, 0);
        init_prime();
    }
    void init_prime() {
        for (int i = 2; i< _sn; ++i) {
            if (!_vis[i]) _prime.push_back(i);
            for (int j = 0, np = _prime.size(); j<np && _prime[j]*i<_sn; ++j) {
                _vis[_prime[j]*i] = 1;
                if (i % _prime[j] == 0) break;
            }
        }
    }
    void divide(int x, vector<int> & factor, vector<int> & cnt) {
        factor.clear();
        cnt.clear();
        int n = 0;
        for (int i = 0, tp = 0, np=_prime.size(); i<np && (tp=_prime[i])*(_prime[i]) <= x; ++i) if(x%tp == 0) {
                factor.push_back(tp);
                n = 0;
                while(x % tp == 0) ++n, x/=tp;
                cnt.push_back(n);
            }
        if(x != 1) {
            factor.push_back(x);
            cnt.push_back(1);
        }
    }
    void divide(int x, map<int,int> & mmp, int k=1) {
        vector<int> factor, cnt;
        divide(x, factor, cnt);
        divide(factor, cnt, mmp, k);
    }
    void divide(vector<int> & factor, vector<int> & cnt, map<int,int> & mmp, int k=1) {
        map<int,int>::iterator it;
        for (int i = 0, l=factor.size(); i<l; ++i) {
            it = mmp.find(factor[i]);
            if (it != mmp.end()) {
                it->second += k*cnt[i];
            } else
                mmp.insert(make_pair(factor[i], k*cnt[i]));
        }
    }
} prime(1000000000);
int ans, com;
int n, m;
vector<int> f, c, o;
void cal1(int x) {
    for (int i = 0; i< f.size(); ++i) {
        while (x % f[i] == 0)
            ++o[i], x /= f[i];
    }
    com = (ll) com*x%m;
}
void extend_gcd(int a,int md,int &x,int &y) {
    if(md==0) {
        x=1;
        y=0;
    } else {
        int t;
        extend_gcd(md,a%md,x,y);
        t=x;
        x=y;
        y=t-(a/md)*y;
    }
}
void cal2(int x) {
    for (int i = 0; i< f.size(); ++i) {
        while (x % f[i] == 0 && o[i])
            --o[i], x /= f[i];
    }
    if (x != 1) {
        int xx, yy;
        extend_gcd(x, m, xx, yy);
        xx = (xx%m + m) %m;
        com = (ll) com*xx%m;
    }
}
int main() {
#ifndef ONLINE_JUDGE
    freopen("in.txt", "r", stdin);
#endif
    while (cin>>n>>m && (n+m)) {
        prime.divide(m, f, c);
        o.assign(f.size(), 0);
        ans = 0;
        com = 1;
        for (int i = 1; i<= n; ++i) {
            cal1(4*i-2);
            cal2(i+1);
            int tp = com;
            for (int j = 0; j<o.size(); ++j) {
                for (int k = 0; k< o[j]; ++k) {
                    tp = (ll) tp*f[j] %m;
                }
            }
            ans += tp;
            ans = ans>=m?ans-m:ans;
        }
        printf("%d\n", ans);
    }
    return 0;
}
