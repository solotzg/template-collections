#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
#define vtr(v) (v).begin(),(v).end()
#define mp(a,b) make_pair(a,b)
#define fi first
#define se second
#define pb(a) push_back(a)
#define _0(x) (!(x))
#define _1(x) (x)
typedef vector<int> VI;
typedef pair<int,int> PII;
typedef vector<PII> VPII;
typedef long long ll;
struct Prime {
    typedef int T;
    T _maxn;
    int _sn;
    vector<int> _vis;
    vector<int> _prime;
    Prime(T maxn):_maxn(maxn) {
        _sn = _maxn+1;
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
} ;
struct MissingLCM {
    int getMin(int N) {
        auto p = new Prime(N);
        int res = N+1;
        for (int num : p->_prime) {
            ll s = 1;
            while (s*num <= N) s *= num;
            int p = s;
            while (s <= N) s += p;
            res = max(res, (int)s);
        }
        return res;
    }
};

int main() {
    auto m = new MissingLCM;
    cout<<m->getMin(1000000)<<endl;
    return 0;
}
