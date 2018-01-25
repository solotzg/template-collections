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

struct RGBTree {
    string exist(vector <string> G);
};
const int N = 15;
int mmp[N][N];
struct edge {
    int u, v, t, nxt;
} e[N*N];
int head[N], cnt;
VI pr, pg, pbb;
void init() {
    cnt = 0;
    mst(head, -1);
}
void add(int u, int v, int t) {
    e[cnt] = {u, v, t, head[u]};
    head[u] = cnt++;
}
int fa[N], cfa[N];
int find(int x) {
    return fa[x] = fa[x]==x ? x : find(fa[x]);
}
inline int cb(int x) {
    int r = 0;
    while (x) {
        ++r;
        x &= x-1;
    }
    return r;
}
int n;
int cal() {
    int tp = (n-1)/3;
    if ((int)pr.size() < tp || (int)pg.size() < tp || (int)pbb.size() < tp) return 0;
    int sz = pr.size();
    int sg = pg.size();
    auto checkg = [&](int gst) -> int {
        rp(i, sg) {
            int pos = pg[i];
            if (bit(gst, i)) {
                int a = find(e[pos].u), b = find(e[pos].v);
                if (a == b) {
                    return 0;
                } else {
                    fa[a] = b;
                }
            }
        }
        for (int o : pbb) {
            int a = find(e[o].u), b = find(e[o].v);
            if (a != b) fa[a] = b;
        }
        int p = find(0);
        rep(i, 1, n) {
            if (p != find(i)) {
                return 0;
            }
        }
        return 1;
    };
    auto check = [&](int stat) -> int {
        rp(i, n) fa[i] = i;
        rp(i, sz) {
            int pos = pr[i];
            if (bit(stat, i)) {
                int a = find(e[pos].u), b = find(e[pos].v);
                if (a == b) {
                    return 0;
                } else {
                    fa[a] = b;
                }
            }
        }
        rp(gst, 1<<sg) if (cb(gst) == tp) {
            memcpy(cfa, fa, sizeof fa);
            if (checkg(gst)) return 1;
            memcpy(fa, cfa, sizeof fa);
        }
        return 0;
    };
    rp(stat, 1<<sz) if (cb(stat) == tp ) {
        if (check(stat)) return 1;
    }
    return 0;
}
string RGBTree::exist(vector <string> g) {
    init();
    ::n = g.size();
    rp(i, g.size()) rep(j, i+1, g.size()) {
        int r = -1;
        switch(g[i][j]) {
        case 'R':
            r = 0;
            break;
        case 'G':
            r = 1;
            break;
        case 'B':
            r = 2;
            break;
        }
        if (r != -1)
            add(i, j, r);
        if (r == 0) {
            pr.pb(cnt-1);
        } else if (r == 1) {
            pg.pb(cnt-1);
        } else if (r == 2)
            pbb.pb(cnt-1);
    }
    if (cal()) return "Exist";
    else return "Does not exist";
}

int main() {
    RGBTree r;
    cout<<r.exist( {
        ".RGB",
        "R...",
        "G...",
        "B..."
    }
                 )<<endl;;
    return 0;
}
