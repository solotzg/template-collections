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
#define bit(x,y) (((x)>>(y))&1)
typedef vector<int> VI;
typedef pair<int,int> PII;
typedef vector<PII> VPII;
typedef long long ll;

double dp[2][10005];
struct CtuRobots {
    double maxDist(int B, vector <int> cost, vector <int> cap) {
        vector<PII> pp(cost.size());
        rp(i, cost.size()) pp[i] = mp(cap[i], cost[i]);
        sort(vtr(pp));
        int cur = 0, pre = 1;
        mst(dp, 0);
        for (auto & a : pp )  {
            cur = pre;
            pre = 1-pre;
            mst(dp[cur], 0);
            rep(j, 0, a.se) dp[cur][j] = dp[pre][j];
            repd(j, a.se, B) {
                dp[cur][j] = max(dp[pre][j - a.se]/3.0 + a.fi, dp[pre][j]);
            }
        }
        return dp[cur][B]/2.0;
    }
};
