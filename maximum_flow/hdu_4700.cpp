#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
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

struct UnionFind {
    vector<int> data;
    void init(int n) {
        data.assign(n, -1);
    }
    bool union_set(int x, int y) {
        x = root(x);
        y = root(y);
        if(x != y) {
            if(data[y] < data[x]) swap(x, y);
            data[x] += data[y];
            data[y] = x;
        }
        return x != y;
    }
    bool find_set(int x, int y) {
        return root(x) == root(y);
    }
    int root(int x) {
        return data[x] < 0 ? x : data[x] = root(data[x]);
    }
    int size(int x) {
        return -data[root(x)];
    }
};

int mmp[105][105];
int res[105][105];
int cal(int n) {
    int inf = 0x3f3f3f3f;
    int f = 1;
    mst(res, 0);
    rp(i, n) rp(j, n) {
        scanf("%d", &mmp[i][j]);
    }
    rp(i, n) mmp[i][i] = inf, res[i][i] = -1;
    rp(a, n) rp(b, n) rp(c, n) if (mmp[a][c] < min(mmp[a][b], mmp[b][c]) ) {
        return 0;
    }
    UnionFind u;
    u.init(n);
    vector<pair<int, PII>> v;
    rp(i, n) rep(j, i+1, n) v.pb(mp(mmp[i][j], mp(i, j) ) );
    sort(vtr(v) );
    reverse(vtr(v) );
    rp(k, v.size()) {
        pair<int, PII> & o = v[k];
        int w = o.fi;
        int i = o.se.fi, j = o.se.se;
        if (u.union_set(i, j) ) {
            res[i][j] = res[j][i] = w;
        }
    }
    return 1;
}


int main() {
#ifdef _TZG_DEBUG
    freopen("in.txt", "r", stdin);
#endif // _TZG_DEBUG
    int n;
    while (scanf("%d", &n) != EOF) {
        if (cal(n)) {
            puts("YES");
            rp(i, n) {
                rp(j, n) {
                    if (j) printf(" ");
                    printf("%d", res[i][j]);
                }
                printf("\n");
            }
        } else puts("NO");
    }
    return 0;
}
