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

const int N = 1e6+5;
struct Prime {
    typedef int T;
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
    void divide(T x, vector<T> & factor) {
        factor.clear();
        T tp;
        for (int i = 0, np=_prime.size(); i<np && (tp=_prime[i], tp*tp) <= x; ++i) if(x%tp == 0) {
                factor.push_back(tp);
                while(x % tp == 0) x/=tp;
            }
        if(x != 1) {
            factor.push_back(x);
        }
    }
} prime((int)1e9);
const ll INF = 0x3f3f3f3f3f3f3f3f;
int num[N];
int n, a, b;
ll dp[2][4], res;
VI ff;
set<int> s;
int main() {
    //freopen("in.txt", "r", stdin);
    cin>>n>>a>>b;
    rp(i, n) scanf("%d", num+i);
    VI o;
    o.pb(num[0]);
    o.pb(num[n-1]);
    res = INF;
    rp(u, o.size()) {
        s.clear();
        repd(d,-1,1) {
            prime.divide(o[u]+d, ff);
            for (int p : ff) if (s.find(p) == s.end()) {
                    s.insert(p);
                    int cur=0, pre=1-cur;
                    mst(dp[cur], 0x3f);
                    dp[cur][0] = d==0?0:b;
                    int bg = u==0?1:0, ed = u==0?n-1:n-2;
                    repd(i, bg, ed) {
                        cur = pre;
                        pre = 1-pre;
                        mst(dp[cur], 0x3f);
                        int e = num[i];
                        if (e % p == 0) {
                            dp[cur][0] = dp[pre][0];
                            dp[cur][1] = min(dp[pre][0]+a, dp[pre][1]+a);
                            dp[cur][2] = min(dp[pre][1], dp[pre][2]);
                        } else {
                            dp[cur][0] = INF;
                            dp[cur][1] = min(dp[pre][0]+a, dp[pre][1]+a);
                            dp[cur][2] = INF;
                            if ((e+1)%p == 0 || (e-1)%p == 0) {
                                dp[cur][0] = min(dp[cur][0], dp[pre][0]+b);
                                dp[cur][2] = min(dp[cur][2], min(dp[pre][1]+b, dp[pre][2]+b));
                            }
                        }
                    }
                    rp(i, 3) res = min(res, dp[cur][i]);
                }
        }
    }
    cout<<res<<endl;
    return 0;
}
