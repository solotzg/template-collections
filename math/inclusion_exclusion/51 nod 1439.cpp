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
    void divide(int x, vector<int> & factor) {
        factor.clear();
        for (int i = 0, tp = 0, np=_prime.size(); i<np && (tp=_prime[i])*(_prime[i]) <= x; ++i) if(x%tp == 0) {
                factor.push_back(tp);
                while(x % tp == 0) x/=tp;
            }
        if(x != 1) {
            factor.push_back(x);
        }
    }
};
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
typedef long long ll;
#define N 200005
Prime prime(500005);
int n, q;
int a[N], cs;
int s[N];
int vn[N], cvn, sn[500005];
ll res;
vector<int> dd[N];
void dfs(int dx, int pos, int val, int ad) {
    if (dx == (int) dd[pos].size()) {
        if (val != 1) {
            vn[cvn++] = val;
            res +=  ad * (ll)sn[val];
        }
        return;
    }
    dfs(dx+1, pos, val, ad);
    dfs(dx+1, pos, val*dd[pos][dx], -ad);
}
void solve() {
    cin>>n>>q;
    repd(i,1,n) scanf("%d", a+i), prime.divide(a[i], dd[i]);
    mst(s, 0);
    mst(sn,0);
    res = 0;
    cs = 0;
    rp(i,q) {
        int x;
        scanf("%d", &x);
        if (!s[x]) {
            if (a[x] == 1) {
                res += cs;
            } else {
                cvn = 0;
                res += cs;
                dfs(0, x, 1, 1);
                rp(j,cvn) ++sn[vn[j]];
            }
            s[x] = 1;
            ++cs;
        } else {
            if (a[x] == 1) {
                res -= cs-1;
            } else {
                cvn = 0;
                res -= cs;
                dfs(0, x, 1, -1);
                rp(j,cvn) --sn[vn[j]];
            }
            s[x] = 0;
            --cs;
        }
        printf("%lld\n", res);
    }
}
int main() {
    freopen("in.txt", "r", stdin);
    solve();
    return 0;
}
