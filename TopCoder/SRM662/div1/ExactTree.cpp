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
struct ExactTree{
    const static int N = 55, M = 105;
    int dp[N][M];
    int getTree(int n, int m, int r){
        mst(dp, -1);
        dp[1][0] = 0;
        repd(s,2,n) {
            rep(a,1,s) {
                rp(pa,m) {
                    int b = s-a;
                    rp(pb,m) {
                        if (dp[a][pa] == -1 || dp[b][pb] == -1) continue;
                        int u = dp[a][pa]+dp[b][pb]+a*(n-a);
                        auto & k = dp[s][u%m];
                        k = k==-1?u:min(k,u);
                    }
                }
            }
        }
        return dp[n][r];
    }
};
int main() {
    auto e = new ExactTree;
    cout<<(e->getTree(4,100,9));
    return 0;
}
