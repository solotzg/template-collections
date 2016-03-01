struct SegmentTree {
#define lr (root<<1)
#define rr (lr|1)
#define lson l,m,lr
#define rson m+1,r,rr
    typedef long long ll;
    const static int N = 100005;
    /*
        root from 1
    */
    struct Node {
        // data
    };
    Node da[N*4];
    void build(int l, int r, int root) {
        // do something
        da[root] = Node();
        if (l == r) return;
        int m = (l+r)/2;
        build(lson);
        build(rson);
    }
    void merge_down(int l, int r, int root) {
        if (l == r) return;
        // ...
    }
    void merge_up(int l, int r, int root) {
        if (l == r) return;
        // ...
    }
    void update(int L, int R, int l, int r, int root) {
        if (L <= l && r <= R) {
            //
            return;
        }
        merge_down(l,r,root);
        int m = (l+r)/2;
        if (m >= L) update(L,R,lson);
        if (m <  R) update(L,R,rson);
        merge_up(l,r,root);
    }
    void query(int L, int R, int l, int r, int root) {
        if (L <= l && r <= R) {
            //
            return;
        }
        merge_down(l,r,root);
        int m = (l+r)/2;
        if (m >= L) query(L,R,lson);
        if (m <  R) query(L,R,rson);
    }
};
