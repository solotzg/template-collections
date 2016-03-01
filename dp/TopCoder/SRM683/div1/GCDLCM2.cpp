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

struct GCDLCM2 {
    typedef int T;
    int _sn;
    vector<int> _vis;
    vector<int> _prime;
    void init() {
        _sn = (int) sqrt(1e7) + 5;
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
    unordered_map<int, VI> mmp;
    void divide(T x) {
        int m;
        T tp;
        for (int i = 0, np=_prime.size(); i<np && (tp=_prime[i], tp*tp) <= x; ++i) if(x%tp == 0) {
                m = 1;
                while(x % tp == 0) x/=tp, m *= tp;
                mmp[tp].pb(m);
            }
        if(x != 1) mmp[x].pb(x);
    }
    const static int N = 1e7+5, MOD = 1e9+7;
    int getMaximalSum(vector <int> start, vector <int> d, vector <int> cnt){
        int n = 0;
        init();
        rp(i,start.size()) rp(j, cnt[i]) {
            int board = (start[i]+j*d[i]);
            ++n;
            divide(board);
        }
        VI sum(n, 1);
        for (auto & e : mmp) {
            auto & v = e.se;
            sort(vtr(v), greater<int>());
            rp(i, v.size()) {
                sum[i] = (ll)sum[i]*v[i]%MOD;
            }
        }
        int res = 0;
        rp(i, n) {
            res += sum[i];
            res = res>=MOD?res-MOD:res;
        }
        return res;
    }
};
int main() {
    GCDLCM2 *g = new GCDLCM2;
    cout<<g->getMaximalSum(
    {5 ,6}, {23, 45}, {50000, 50000}
    )<<endl;
    return 0;
}
