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
struct LineMSTDiv2{
    int count(int n) {
        const int MOD = 1e9+7;
        const static int N = 20;
        static int e[N][N];
        int res = 0;
        rp(i, (1<<(n-1))) {
            rp(u,n) rep(v,u+1,n) e[u][v] = 1;
            rp(j,n-1) {
                if ((1<<j)&i) {
                    repd(u,0,j) rep(v,j+1,n) e[u][v] = 2;
                }
            }
            int sum = 1;
            rp(u,n) rep(v,u+2,n) if ( e[u][v] == 1) sum = sum*2ll % MOD;
            res = (res+sum)%MOD;
        }
        repd(i,3,n) res = (ll)res*i%MOD;
        return res;
    }
};
int main() {
    auto a = new LineMSTDiv2;
    assert(a->count(3) == 15);
    assert(a->count(16) == 682141922);
    return 0;
}

