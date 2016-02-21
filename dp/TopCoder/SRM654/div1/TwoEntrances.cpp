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

struct TwoEntrances {
    const static int N = 3005, MOD = 1e9+7;
    inline int mul(int a, int b) {
        return (ll)a*b%MOD;
    }
    inline int add(int a, int b) {
        return (a+=b)>=MOD?a-MOD:(a<0?a+MOD:a);
    }
    int comb[N][N];
    void init() {
        repd(i,0,n) {
            comb[i][0]=comb[i][i]=1;
            rep(j,1,i) comb[i][j]=add(comb[i-1][j-1], comb[i-1][j]);
        }
        mst(dp, -1);
    }
    int n;
    VI edge[N], path;
    int pre[N];
    void getPre(int s1) {
        mst(pre, -1);
        queue<int> q;
        q.push(s1);
        while (!q.empty()) {
            int o = q.front();
            q.pop();
            for (int v : edge[o]) if (v != pre[o])
                    q.push(v), pre[v]=o;
        }
    }
    void getPath(int s2) {
        int p = s2;
        while (p != -1)
            path.pb(p), p = pre[p];
    }
    int treeCnt[N], ways[N];
    void dfs_no_root (int u, int fa) {
        treeCnt[u] = 1;
        ways[u] = 1;
        for (int v : edge[u]) if (v != fa) {
                dfs_no_root (v, u);
                treeCnt[u] += treeCnt[v];
            }
        int all = treeCnt[u]-1;
        for (int v : edge[u]) if (v != fa) {
                ways[u] = mul(ways[u], mul(ways[v], comb[all][treeCnt[v]]));
                all -= treeCnt[v];
            }
    }
    void dfs_root(int dx) {
        int u = dx==0?-1:path[dx-1];
        int v = (dx+1==(int)path.size())?-1:path[dx+1];
        int o = path[dx];
        treeCnt[o] = 1;
        ways[o] = 1;
        for (int e : edge[o]) if (e != u && e != v) {
                dfs_no_root (e, o);
                treeCnt[o] += treeCnt[e];
            }
        int all = treeCnt[o]-1;
        for (int e : edge[o]) if (e != u && e != v) {
                ways[o] = mul(ways[o], mul(ways[e], comb[all][treeCnt[e]]));
                all -= treeCnt[e];
            }
    }
    int dp[N][N];
    int calc(int bg, int ed) {
        urp(i,ed,bg) {
            int all = 0;
            repd(j,i,ed) {
                all += treeCnt[path[j]];
                if (i == j) dp[i][j] = ways[path[j]];
                else {
                    int & d = dp[i][j];
                    d = 0;
                    int ca = 1, cb = 1;
                    ca = mul(ways[path[i]], comb[all-1][treeCnt[path[i]]-1]);
                    ca = mul(ca, dp[i+1][j]);

                    cb = mul(ways[path[j]], comb[all-1][treeCnt[path[j]]-1]);
                    cb = mul(cb, dp[i][j-1]);

                    d = add(ca, cb);
                }
            }
        }
        return dp[bg][ed];
    }
    int count(VI a, VI b, int s1, int s2) {
        n = a.size()+1;
        init();
        rp(i,a.size()) edge[a[i]].pb(b[i]), edge[b[i]].pb(a[i]);
        getPre(s1);
        getPath(s2);
        rp(i,path.size()) dfs_root(i);
        return calc(0, path.size()-1);
    }
};


int main() {
    unique_ptr<TwoEntrances> p(new TwoEntrances());
    cout<<p->count(

    {0, 0, 1, 2, 3, 1, 2, 0, 6, 5, 10, 10},
    {1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10, 11, 12},
    3,
    6
        )<<endl;
    return 0;
}
