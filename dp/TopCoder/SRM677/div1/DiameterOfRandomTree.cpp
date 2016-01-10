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
typedef vector<int> VI;
typedef pair<int, int> PII;
typedef vector<PII> VPII;
typedef long long ll;
struct DiameterOfRandomTree {
    const static int N = 128;
    int max_diameter, n2;
    double dp[N][N];
    VI g[N];
    void dfs(int u, int pre) {
        static double p[N];
        int v;
        rp(i, g[u].size()) if((v = g[u][i]) != pre) dfs(v, u);
        mst(dp[u], 0);
        dp[u][0] = 1.0; // init
        // if u has no child, dp[u][0] exists
        rp(j, g[u].size()) if((v = g[u][j]) != pre) {
            mst(p, 0);
            repd(a, 0, n2) repd(b, 0, n2) {
                if(a + b + 1 <= max_diameter) p[max(a,b + 1)] += dp[u][a] * dp[v][b] * 0.5;
                if(a + b + 2 <= max_diameter) p[max(a,b + 2)] += dp[u][a] * dp[v][b] * 0.5;
            }
            memcpy(dp[u], p, sizeof p);
        }
    }
    double getExpectation(vector <int> a, vector <int> b) {
        rp(i, a.size()) g[a[i]].pb(b[i]), g[b[i]].pb(a[i]);
        n2 = 2 * a.size();
        double pre = 0.0, cur, res = 0.0;
        repd(i, 1, n2) {
            max_diameter = i;
            dfs(0, -1);
            cur = 0.0;
            repd(j, 1, n2) cur += dp[0][j];
            res += i * (cur - pre);
            pre = cur;
        }
        return res;
    }
};
int main() {
    DiameterOfRandomTree *d = new DiameterOfRandomTree;
    assert(d->getExpectation(	{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49}, {0, 0, 1, 3, 1, 1, 5, 5, 7, 8, 10, 6, 9, 8, 4, 4, 9, 14, 9, 0, 4, 0, 14, 2, 2, 3, 18, 27, 2, 22, 13, 31, 27, 18, 32, 16, 14, 19, 34, 39, 20, 38, 34, 17, 0, 36, 7, 10, 37}
    ) == 19.064013742958196);
    return 0;
}
