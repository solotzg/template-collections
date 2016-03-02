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
struct TreeAndPathLength2{
    int vis[53][53][1005];
    int nn, ss;
    void dfs(int n, int p, int s) {
        if (vis[n][p][s]) return;
        vis[n][p][s] = 1;
        for(int i = 1; i+n <= nn; ++i) {
            int o = s + i + i*(i-1)/2;
            if (o <= ss)
                dfs(n+i, p-1+i, o);
        }
    }
    string possible(int n, int s) {
        nn = n;
        ss = s;
        mst(vis, 0);
        repd(i,1,n-1) {
            int p = i*(i-1)/2;
            if (p <= s)
                dfs(i+1, i, p);
        }
        repd(i,1,n) if (vis[n][i][s]) return "Possible";
        return "Impossible";
    }
};
int main() {
    return 0;
}

