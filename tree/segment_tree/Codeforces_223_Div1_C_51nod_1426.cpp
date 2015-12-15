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
const int N = 1000005;
char s[N];
struct Node {
    int nl, nr, np;
    Node(int _nl = 0, int _nr = 0, int _np = 0):nl(_nl),nr(_nr),np(_np) {}
};
struct SegmentTree {
#define lr (root<<1)
#define rr (lr|1)
#define lson l,m,lr
#define rson m+1,r,rr
    typedef long long ll;
    const static int N = 1000005;
    /*
        root from 1
    */
    Node da[N*4];
    void build(int l, int r, int root) {
        // do something
        if (l == r) {
            da[root] = Node(s[l]=='(',s[l]==')',0);
            return;
        }
        int m = (l+r)/2;
        build(lson);
        build(rson);
        da[root] = calc(da[lr], da[rr]);
    }
    Node calc(Node & a, Node & b) {
        int tp = min(a.nl, b.nr);
        return Node(
                   a.nl+b.nl-tp,
                   a.nr+b.nr-tp,
                   a.np+b.np+tp
               );
    }
    Node query(int L, int R, int l, int r, int root) {
        if (L <= l && r <= R) {
            return da[root];
        }
        int m = (l+r)/2;
        Node n1, n2;
        if (m >= L) n1 = query(L,R,lson);
        if (m <  R) n2 = query(L,R,rson);
        return calc(n1,n2);
    }
} sg;
int main() {
    //freopen("in.txt", "r", stdin);
    scanf("%s", s);
    int len = strlen(s);
    sg.build(0, len-1, 1);
    int q;
    scanf("%d", &q);
    while (q--) {
        int l, r;
        scanf("%d%d", &l, &r);
        Node nd = sg.query(l-1,r-1,0,len-1,1);
        printf("%d\n", 2*nd.np);
    }
    return 0;
}
