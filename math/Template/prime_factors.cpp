#include <cstdio>
#include <complex>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <cstring>
using namespace std;

struct PrimeForDivide {
    typedef int T;
    // N is not very big
    const static int N = 1e6+5;
    int mxPrime[N];
    PrimeForDivide() {
        init_prime();
    }
    void init_prime() {
        memset(mxPrime, 0, sizeof mxPrime);
        for (int i = 2; i< N; ++i) {
            if (mxPrime[i]) continue;
            for (int j = i+i; j< N; j += i) {
                mxPrime[j] = i;
            }
        }
    }
    void divide(T x, T factor[], int cnt[], int & len) {
        len = 0;
        while (x > 1) {
            int j = mxPrime[x], c = 0;
            factor[len] = j;
            while ( x % j == 0) x /= j, ++c;
            cnt[len++] = c;
        }
    }
};

struct Prime {
    typedef long long T;
    T _maxn;
    int _sn;
    vector<int> _vis;
    vector<int> _prime;
    Prime(T maxn):_maxn(maxn) {
        _sn = (int) sqrt((double) _maxn) + 5;
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
    void divide(T x, vector<T> & factor, vector<int> & cnt) {
        factor.clear();
        cnt.clear();
        int n = 0;
        T tp;
        for (int i = 0, np=_prime.size(); i<np && (tp=_prime[i], tp*tp) <= x; ++i) if(x%tp == 0) {
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
    void divide(T x, T factor[], int cnt[], int & len) {
        len = 0;
        int n = 0;
        T tp;
        for (int i = 0, np=_prime.size(); i<np && (tp=_prime[i], tp*tp) <= x; ++i) if(x%tp == 0) {
                factor[len] = tp;
                n = 0;
                while(x % tp == 0) ++n, x/=tp;
                cnt[len++] = n;
            }
        if(x != 1) {
            factor[len] = x;
            cnt[len++] = 1;
        }
    }
    void divide(T x, map<T,int> & mmp, int k=1) {
        vector<T> factor;
        vector<int> cnt;
        divide(x, factor, cnt);
        divide(factor, cnt, mmp, k);
    }
    void divide(vector<T> & factor, vector<int> & cnt, map<T,int> & mmp, int k=1) {
        map<T,int>::iterator it;
        for (int i = 0, l=factor.size(); i<l; ++i) {
            it = mmp.find(factor[i]);
            if (it != mmp.end()) {
                it->second += k*cnt[i];
            } else
                mmp.insert(make_pair(factor[i], k*cnt[i]));
        }
    }
} prime(10000000000ll);
struct FactorDFS {
    const static int N = 105;
    long long factor[N];
    int cnt[N], len;
    void dfs(int pos, long long val) {
        if (pos == len) {
            // fuck something
            return;
        }
        for (int i = 0; i<= cnt[pos]; ++i) {
            dfs(pos+1, val);
            val *= factor[pos];
        }
    }
};
int main() {
    typedef long long T;
    vector<T> f;
    vector<int> c;
    map<T,int> mmp;
    T a = 100224546;
    prime.divide(a, f, c);
    T sum = 1;
    for (int i = 0; i< (int)f.size(); ++i) {
        cout<<"factor : "<<f[i]<<", count : "<<c[i]<<endl;
        for (int j = 0; j< c[i]; ++j) {
            sum *= f[i];
        }
    }
    cout<<"origin : "<<a<<" , result : "<<sum<<endl<<endl;
    prime.divide(a, mmp);
    sum = 1;
    for (map<T,int>::iterator it = mmp.begin(); it != mmp.end(); ++it) {
        cout<<"factor : "<<it->first<<", count : "<<it->second<<endl;
        for (int j = 0; j< it->second; ++j) {
            sum *= it->first;
        }
    }
    cout<<"origin : "<<a<<" , result : "<<sum<<endl<<endl;
    a /= 6;
    prime.divide(6, mmp, -1);
    sum = 1;
    for (map<T,int>::iterator it = mmp.begin(); it != mmp.end(); ++it) {
        cout<<"factor : "<<it->first<<", count : "<<it->second<<endl;
        for (int j = 0; j< it->second; ++j) {
            sum *= it->first;
        }
    }
    cout<<"origin : "<<a<<" , result : "<<sum<<endl<<endl;

    a = 1;
    for (int i = 0; i< prime._sn && a < 1e10; a*=prime._prime[i++]);
    prime.divide(a, f, c);
    sum = 1;
    for (int i = 0; i< (int)f.size(); ++i) {
        cout<<"factor : "<<f[i]<<", count : "<<c[i]<<endl;
        for (int j = 0; j< c[i]; ++j) {
            sum *= f[i];
        }
    }
    cout<<"origin : "<<a<<" , result : "<<sum<<endl<<endl;
    {
        int x = 656744, len;
        int *fa = new int [(int)1e6];
        int *cn = new int [(int)1e6];
        PrimeForDivide *p = (new PrimeForDivide);
        p->divide(x, fa, cn, len);
        int y = 1;
        for (int i = 0; i< len; ++i) {
            cout<<"factor : "<<fa[i]<<", count : "<<cn[i]<<endl;
            for (int j = 0; j< cn[i]; ++j) {
                y *= fa[i];
            }
        }
        cout<<"origin : "<<x<<" , result : "<<y<<endl<<endl;
        delete []fa;
        delete []cn;
        delete p;
    }
    return 0;
}
