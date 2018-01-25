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

struct RookGraph {
    const static int N = 55, MOD=1e9+7;
    void add(int & a, int b) {
        a += b;
        if (a >= MOD) a -= MOD;
    }
    void mul(int & a, int b) {
        a = (ll)a*b%MOD;
    }
    int g[N][N], taken[N][N], vis[N], ele[N][2], tp[N][2], ce, dp[N][N*2][2*N], A[N];
    int k, n, ok;
    PII pos[N];
    PII get_next(int dx) {
        rp(i,n)rp(j,n)if(!taken[i][j]) {
            int f = 1;
            rp(o,k) if (pos[o].fi != -1) {
                if (g[dx][o] == 1 ) {
                    if (i != pos[o].fi && j != pos[o].se) {
                        f = 0;
                        break;
                    }
                } else {
                    if (i == pos[o].fi || j == pos[o].se) {
                        f = 0;
                        break;
                    }
                }
            }
            if (f) return mp(i,j);
        }
        return mp(-1,-1);
    }
    void dfs(int dx) {
        if (!ok) return;
        PII p = get_next(dx);
        if (p.fi == -1) {
            ok = 0;
            return ;
        }
        pos[dx] = p;
        vis[dx] = 1;
        taken[p.fi][p.se] = 1;
        rp(i,k)if(g[dx][i]==1 && !vis[i])  {
            dfs(i);
            if (!ok) return;
        }
    }
    void init() {
        A[0]=1;
        rep(i,1,N) A[i]=(ll)A[i-1]*(n-i+1)%MOD;
    }
    int countPlacements(int _n, vector<string> graph) {
        k = graph.size();
        n = _n;
        if (k > n*n) return 0;
        init();
        rp(i,k)rp(j,k) g[i][j]=graph[i][j]-'0';
        mst(vis, 0);
        ok = 1;
        ce = 0;
        rp(i,k) if (!vis[i]) {
            mst(taken, 0);
            mst(pos, -1);
            dfs(i);
            if (!ok) return 0;
            int cx = 0, cy = 0;
            mst(tp, 0);
            rp(u,n)rp(v,n) tp[u][0] |= taken[u][v], tp[v][1] |= taken[u][v];
            rp(u,n) cx += tp[u][0], cy += tp[u][1];
            ele[ce][0]=cx;
            ele[ce++][1]=cy;
        }
        mst(dp, 0);
        dp[0][0][0] = 1;
        rp(i,ce) {
            repd(s1, 0, n) repd(s2, 0, n) if (dp[i][s1][s2]) {
                add(dp[i+1][s1+ele[i][0]][s2+ele[i][1]], dp[i][s1][s2]);
                if (ele[i][1] != ele[i][0] || ele[i][0]!=1)
                    add(dp[i+1][s1+ele[i][1]][s2+ele[i][0]], dp[i][s1][s2]);
            }
        }
        int res = 0;
        repd(x, 0, n) repd(y, 0, n) if (dp[ce][x][y] ) {
            int p = 1, s1=1, s2=1;
            mul(s1, A[x]);
            mul(s2, A[y]);
            mul(p, s1);
            mul(p, s2);
            mul(p, dp[ce][x][y]);
            add(res, p);
        }
        return res;
    }
};
int main() {
    RookGraph *r = new RookGraph;
    cout<<r->countPlacements(

    8, {
        "111",
        "110",
        "101"
    }
        )<<endl;
    return 0;
}

