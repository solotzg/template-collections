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
struct CollectingTokens {
    const static int N = 55;
    vector<int> e[N], g[N];
    int n;
    int dp[N][N][105][2];
    vector<int> token;
    int f(int u, int dx, int len, int must) {
        int & res = dp[u][dx][len][must];
        int tmp;
        if (res == -1) {
            res = 0;
            if (dx != g[u].size() ) {
                int v = g[u][dx];
                for (int sbl = 0; sbl <= len; ++sbl) {
                    if (len - sbl -2 >= 0) {
                        tmp = token[v] + f(v, 0, sbl, 1) + f(u, dx+1, len-sbl-2, must);
                        res = max(res, tmp);
                    }
                    if (len - sbl - 1 >= 0 && must == 0) {
                        tmp = token[v] + f(v, 0, sbl, 0) + f(u, dx+1, len-sbl-1, 1);
                        res = max(res, tmp);
                    }
                }
                res = max(res, f(u, dx+1, len, must));
            }
        }
        return res;
    }
    void dfs(int u, int pre) {
        for (int v : e[u]) if (v != pre) {
            g[u].pb(v);
            dfs(v, u);
        }
    }
    int maxTokens(vector <int> A, vector <int> B, vector <int> tokens, int L) {
        n = tokens.size();
        rp(i, n-1) {
            e[A[i]-1].pb(B[i]-1);
            e[B[i]-1].pb(A[i]-1);
        }
        dfs(0, -1);
        token = tokens;
        mst(dp, -1);
        return tokens[0] + f(0, 0, L, 0);
    }
};
int main() {
    CollectingTokens * c = new CollectingTokens;
    assert(c->maxTokens({9,1,7,10,5,8,3,4,2},{6,6,9,6,6,1,1,6,6},{4,2,1,6,3,7,8,5,2,9},4) == 26);
    return 0;
}

