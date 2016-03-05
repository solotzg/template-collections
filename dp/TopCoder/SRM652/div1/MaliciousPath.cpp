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

struct MaliciousPath{
    const static int N = 1005;
    const static ll INF = 0x3f3f3f3f3f3f3f3f;
    list<PII> eg[N];
    ll low[N], dis[N];
    int n;
    void cal() {
        mst(dis, 0x3f);
        priority_queue<pair<ll,int> > q;
        q.push(mp(0, n-1));
        dis[n-1] = 0;
        while (!q.empty()) {
            auto e = q.top();
            q.pop();
            ll d = -e.fi;
            int u = e.se;
            if (dis[u] != d) continue;
            for (PII a : eg[u]) {
                int v = a.fi;
                ll mxd = max(low[v], dis[u]+a.se);
                if (mxd < dis[v]) {
                    q.push(mp(-mxd, v));
                    dis[v] = mxd;
                }
            }
        }
    }
    ll minPath(int N, int K, VI from, VI to, VI cost){
        rp(i, from.size()) eg[to[i]].pb(mp(from[i], cost[i]));
        n = N;
        mst(low, 0);
        repd(i,0,K) {
            cal();
            rp(a, n) {
                for (PII b : eg[a]) {
                    low[b.fi] = max(low[b.fi], dis[a]+b.se);
                }
            }
        }
        return dis[0]==INF?-1:dis[0];
    }
};
