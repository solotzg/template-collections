#include <vector>
#include <algorithm>
#include <cstdio>
#include <iostream>
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

const static int N = 300005, M = 1e7;
int n, m;
int A[N];
int phi[M+1], vis[M+1], prime[M], np;
void init() {
    phi[1] = 1;
    repd(i,2,M) {
        if (!vis[i]) {
            prime[np++] = i;
            phi[i] = i-1;
        }
        for (int j = 0, tp; j<np && (ll)prime[j]*i<=M; ++j) {
            tp = prime[j]*i;
            vis[tp] = 1;
            if (i % prime[j] == 0) {
                phi[tp] = prime[j]*phi[i];
                break;
            } else {
                phi[tp] = phi[i]*(prime[j]-1);
            }
        }
    }
}
struct SegmentTree {
#define lr (root<<1)
#define rr (lr|1)
#define lson l,m,lr
#define rson m+1,r,rr
    /*
        root from 1
    */
    struct Node {
        // data
        ll sum;
        int f;
        Node():sum(0),f(0) {}
    };
    Node da[N*4];
    void build(int l, int r, int root) {
        if (l == r) {
            da[root].sum = da[root].f = A[l];
            return;
        }
        int m = (l+r)/2;
        build(lson);
        build(rson);
        merge_up(l,r,root);
    }
    void merge_up(int l, int r, int root) {
        if (l == r) return;
        da[root].sum = da[lr].sum+da[rr].sum;
        da[root].f = da[lr].f == da[rr].f ? da[lr].f : 0;
    }
    void merge_down(int l, int r, int root) {
        if (l == r) return;
        if (!da[root].f) return;
        da[lr].f = da[rr].f = da[root].f;
        int m = (l+r)/2;
        da[lr].sum = (ll) da[lr].f * (m-l+1);
        da[rr].sum = (ll) da[rr].f * (r-m);
    }
    void update(int L, int R, int l, int r, int root) {
        if (L <= l && r <= R && da[root].f) {
            da[root].f = phi[da[root].f];
            da[root].sum = (ll) da[root].f * (r-l+1);
            return;
        }
        merge_down(l,r,root);
        int m = (l+r)/2;
        if (m >= L) update(L,R,lson);
        if (m <  R) update(L,R,rson);
        merge_up(l,r,root);
    }
    void update2(int L, int R, int l, int r, int root, int x) {
        if (L <= l && r <= R) {
            da[root].f = x;
            da[root].sum = (ll) da[root].f * (r-l+1);
            return;
        }
        merge_down(l,r,root);
        int m = (l+r)/2;
        if (m >= L) update2(L,R,lson,x);
        if (m <  R) update2(L,R,rson,x);
        merge_up(l,r,root);
    }
    ll query(int L, int R, int l, int r, int root) {
        if (L <= l && r <= R) {
            return da[root].sum;
        }
        merge_down(l,r,root);
        ll s = 0ll;
        int m = (l+r)/2;
        if (m >= L) s += query(L,R,lson);
        if (m <  R) s += query(L,R,rson);
        return s;
    }
} seg;

int main() {
#ifndef ONLINE_JUDGE
    freopen("in.txt", "r", stdin);
#endif // ONLINE_JUDGE
    int t;
    cin>>t;
    init();
    while (t--) {
        cin>>n>>m;
        repd(i,1,n) scanf("%d", A+i);
        seg.build(1, n, 1);
        int s, l, r, x;
        while (m--) {
            scanf("%d", &s);
            if (s == 1) {
                scanf("%d%d", &l, &r);
                seg.update(l,r,1,n,1);
            } else if (s== 2) {
                scanf("%d%d%d", &l, &r, &x);
                seg.update2(l,r,1,n,1,x);
            } else {
                scanf("%d%d", &l, &r);
                printf("%I64d\n", seg.query(l,r,1,n,1));
            }
        }
    }
    return 0;
}
