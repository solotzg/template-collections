#include <cstdio>
#include <cstring>
#include <algorithm>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
typedef long long ll;
const static int N = 100005;
const ll inf = 0x3f3f3f3f3f3f3f3f;
int V[N];
struct SegmentTree {
#define lr (root<<1)
#define rr (lr|1)
#define lson l,m,lr
#define rson m+1,r,rr
#define mid ((l+r)/2)
    /*
        root from 1
    */
    struct Node {
        ll sum[2][2];
        int sgmax;
        Node(int _sgmax = 0):sgmax(_sgmax){}
    };
    Node da[N*3];
    void build(int l, int r, int root) {
        // do something
        Node & tp = da[root];
        tp = Node();
        if (l == r) {
            tp.sgmax = V[l];
            tp.sum[0][0] = 0;
            tp.sum[1][1] = V[l];
            tp.sum[0][1] = -inf;
            tp.sum[1][0] = -inf;
            return;
        }
        int m = mid;
        build(lson);
        build(rson);
        merge_up(da[root], da[lr], da[rr]);
    }
    void merge_up(Node & tp, const Node & n1, const Node & n2) {
        tp.sgmax = max(n1.sgmax, n2.sgmax);
        rp(i,4) {
            ll & cao = tp.sum[i/2][i%2];
            cao = -inf;
            rp(j,3){
                cao = max(cao, n1.sum[i/2][j/2] + n2.sum[j%2][i%2]);
            }
        }
    }
    void update(int p, int v, int l, int r, int root) {
        if (l == r) {
            Node & tp = da[root];
            tp.sgmax = tp.sum[1][1] = v;
            return;
        }
        int m = mid;
        if (m >= p) update(p,v,lson);
        if (m <  p) update(p,v,rson);
        merge_up(da[root], da[lr], da[rr]);
    }
    Node query(int L, int R, int l, int r, int root) {
        if (L <= l && r <= R) {
            return da[root];
        }
        int m = mid;
        Node n1, n2, res;
        int check[2] = {0,0};
        if (m >= L) n1 = query(L,R,lson), check[0] = 1;
        if (m <  R) n2 = query(L,R,rson), check[1] = 1;
        if (!check[0]) return n2;
        if (!check[1]) return n1;
        merge_up(res, n1, n2);
        return res;
    }
    ll query(int l, int r, int n) {
        Node res = query(l, r, 0, n-1, 1);
        ll o = res.sgmax < 0 ? res.sgmax : max(res.sum[0][0], (ll)res.sgmax);
        rep(i,1,4) {
            o = max(o, res.sum[i/2][i%2]);
        }
        return o;
    }
}seg;
int n, q;
int main() {
    freopen("in.txt", "r", stdin);
    scanf("%d%d", &n, &q);
    rp(i,n) scanf("%d", V+i);
    seg.build(0, n-1, 1);
    while(q--){
        int c, l, r;
        scanf("%d", &c);
        if (c == 1) {
            scanf("%d%d", &l, &r);
            seg.update(l, r, 0, n-1, 1);
        } else {
            scanf("%d%d", &l, &r);
            printf("%lld\n", seg.query(l, r, n));
        }
    }
    return 0;
}
