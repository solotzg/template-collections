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
struct ColorfulLineGraphs {
    ll pow(ll x, ll n, ll m) {
        ll r = 1, p = x;
        while (n) {
            if (n & 1) r = r*p%m;
            p = p * p %m;
            n >>= 1;
        }
        return r;
    }
    int countWays(long long N, long long K, int M) {
        int a = 1, b = 1;
        rp(i,M) {
            if (i == N%M) b = a;
            a = a*(K+i*(K-1)%M)%M;
        }
        return pow(a,N/M,M)*b%M;
    }
};
int main() {
    auto a = new ColorfulLineGraphs;
    cout<<a->countWays(5000,1000000000000ll,314159)<<endl;
    return 0;
}
