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

struct Autohamil  {
    string check(vector <int> z0, vector <int> z1);
};
const int N = 55;
struct edge{
    int v, nxt;
}e[N*4];
int ecnt, head[N];
void add(int u, int v){
    e[ecnt].v = v; e[ecnt].nxt = head[u]; head[u] = ecnt++;
}
int dfn[N], low[N], cnt, isin[N];
int cfz[N], nfz, fz[N];
int mstk[N], top;
void init() {
    mst(head, -1);
    ecnt = 0;
    mst(dfn, 0);
    mst(low, 0);
    cnt = 0;
    mst(isin, 0);
    top = 0;
    nfz = 0;
    mst(cfz, 0);
}
void cal(int u) {
    dfn[u] = low[u] = ++cnt;
    isin[u] = 1;
    mstk[++top] = u;
    for (int i = head[u]; ~i; i=e[i].nxt) {
        int v = e[i].v;
        if (!dfn[v]) {
            cal(v);
            low[u] = min(low[v], low[u]);
        } else if(isin[v]) {
            low[u] = min(low[u], dfn[v]);
        }
    }
    if (dfn[u] == low[u]) {
        int v = -1;
        do {
            v = mstk[top];
            isin[v] = 0;
            cfz[nfz]++;
            --top;
            fz[v] = nfz;
        }while (u != v);
        ++nfz;
    }
}
int n;
int mmp[N][N], vis[N];
int dfs(int u) {
    if (vis[u] == nfz) return 1;
    rp(v, nfz) if (mmp[u][v] && vis[v] == 0) {
        vis[v] = vis[u]+1;
        if (dfs(v)) return 1;
        vis[v] = 0;
    }
    return 0;
}
int go() {
    rp(i, n) if (low[i] == 0)
        cal(i);
    mst(mmp, 0);
    mst(vis, 0);
    rp(u, n) {
        for (int i = head[u]; ~i; i=e[i].nxt){
            int v = e[i].v;
            if (fz[u] != fz[v]) mmp[fz[u]][fz[v]] = 1;
        }
    }
    vis[fz[0]] = 1;
    if (dfs(fz[0])) return 1;
    return 0;
}
string Autohamil::check(vector <int> z0, vector <int> z1){
    init();
    n = z0.size();
    rp(i, z0.size()) {
        add(i, z0[i]);
        add(i, z1[i]);
    }
    if (go()) return "Exists";
    return "Does not exist";
}


int main() {
    Autohamil a;
    cout<<a.check(
        {0,1},
        {0,1}
    )<<endl;
    return 0;
}
