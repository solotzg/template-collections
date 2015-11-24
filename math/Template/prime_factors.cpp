#include <cstdio>
#include <complex>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
using namespace std;
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
int main() {
    vector<int> f, c;
    map<int,int> mmp;
    int a = 100224546;
    prime.divide(a, f, c);
    int sum = 1;
    for (int i = 0; i< f.size(); ++i) {
        printf("factor : %d, count : %d\n", f[i], c[i]);
        for (int j = 0; j< c[i]; ++j) {
            sum *= f[i];
        }
    }
    printf("origin : %d , result : %d\n\n", a, sum);
    prime.divide(a, mmp);
    sum = 1;
    for (map<int,int>::iterator it = mmp.begin(); it != mmp.end(); ++it) {
        printf("factor : %d, count : %d\n", it->first, it->second);
        for (int j = 0; j< it->second; ++j) {
            sum *= it->first;
        }
    }
    printf("origin : %d , result : %d\n\n", a, sum);
    a /= 6;
    prime.divide(6, mmp, -1);
    sum = 1;
    for (map<int,int>::iterator it = mmp.begin(); it != mmp.end(); ++it) {
        printf("factor : %d, count : %d\n", it->first, it->second);
        for (int j = 0; j< it->second; ++j) {
            sum *= it->first;
        }
    }
    printf("origin : %d , result : %d\n\n", a, sum);
    return 0;
}
