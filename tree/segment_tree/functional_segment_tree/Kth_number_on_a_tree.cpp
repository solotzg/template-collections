#include <cstdio>
#include <iostream>
#include <cmath>
#include <map>
#include <algorithm>
#include <vector>
#include <cstring>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
const int N = 100010;
typedef long long LL;
struct FuncSegTree {
        // 区间第k大
    struct Node{
        int sum, lson, rson;
        Node(int _sum = 0, int _lson = 0, int _rson = 0):
            sum(_sum), lson(_lson), rson(_rson){}
    };
    Node tree[N*20];
    int cnt;
    void init(int & root, int l, int r) {
        cnt = 0;
        build(root, l, r);
    }
    void build(int & root, int l, int r) {
        root = new_node(0, l, r);
        if (l == r) return;
        int m = (l+r)/2;
        build(tree[root].lson, l, m);
        build(tree[root].rson, m+1, r);
    }
    int new_node(int sum, int lson, int rson) {
        tree[++cnt] = Node(sum, lson, rson);
        return cnt;
    }
    void insert(int & root, int pre, int pos, int l, int r) {
            //从根节点往下更新到叶子，新建立出一路更新的节点，这样就是一颗新树了。
        root = new_node(tree[pre].sum+1, tree[pre].lson, tree[pre].rson);
        if (l == r) return;
        int m = (l+r)/2;
        if (pos <= m)
            insert(tree[root].lson, tree[pre].lson, pos, l, m);
        else
            insert(tree[root].rson, tree[pre].rson, pos, m+1, r);
    }
    int query(int u, int v, int lca, int lca_fa, int k, int l, int r) {
        if ( l == r )
            return l;
        int m = (l+r)/2;
            //下面计算的sum就是当前询问的区间中，左儿子中的元素个数
        int sum = tree[tree[u].lson].sum + tree[tree[v].lson].sum
            - tree[tree[lca].lson].sum - tree[tree[lca_fa].lson].sum;
        if ( k <= sum )
            return query( tree[u].lson, tree[v].lson,
                          tree[lca].lson, tree[lca_fa].lson,
                          k, l, m);
        else
            return query( tree[u].rson, tree[v].rson,
                          tree[lca].rson, tree[lca_fa].rson,
                          k-sum, m+1, r);
    }
}func_seg_tree;
struct Edge {
    int to, nxt;
    Edge(int _to=-1, int _nxt=-1):to(_to),nxt(_nxt) {}
};
Edge edge[N*2];
int head[N], ecnt;
void tree_init() {
    memset(head, -1, sizeof head);
    ecnt = 0;
}
void tree_add(int u, int v) {
    edge[ecnt] = Edge(v,head[u]), head[u] = ecnt++;
}
int query[N][3];
vector<int> lca_ps[N], lca_qr[N];
int lca_vis[N], lca_fa[N], lca_ans[N];
int find(int x) {
    return lca_fa[x]=lca_fa[x]==x?x:find(lca_fa[x]);
}
void lca_dfs(int u, int pre) {
    lca_fa[u] = u;
    for (int i = head[u], v; i!=-1; i=edge[i].nxt) {
        if ((v=edge[i].to) != pre) {
            lca_dfs(v, u);
            lca_fa[v] = u;
        }
    }
    lca_vis[u] = 1;
    rp(i, lca_qr[u].size()) {
        int v = lca_qr[u][i];
        if (lca_vis[v]) {
            lca_ans[lca_ps[u][i]] = find(v);
        }
    }
}
void lca_init(int n) {
    repd(i,1,n) {
        lca_vis[i] = 0;
        lca_ps[i].clear();
        lca_qr[i].clear();
    }
}
int n, m, p;
int nums[N], sorted[N], root[N], fa[N];
void dfs(int u, int pre){
    fa[u] = pre;
    int dx = lower_bound(sorted+1, sorted+1+p, nums[u]) - sorted;
    func_seg_tree.insert(root[u], root[pre], dx, 1, p);
    for (int i = head[u]; i!=-1; i=edge[i].nxt) {
        if (edge[i].to != pre)
            dfs(edge[i].to, u);
    }
}
int main() {
    freopen("in.txt", "r", stdin);
    int u, v, k;
    cin>>n>>m;
    repd(i,1,n)
        scanf("%d", nums+i), sorted[i] = nums[i];
    sort(sorted+1, sorted+n+1);
    p = unique(sorted+1, sorted+n+1) - (sorted+1);
    tree_init();
    func_seg_tree.init(root[0], 1, p);
    rp(i,n-1) {
        scanf("%d%d", &u, &v);
        tree_add(u, v);
        tree_add(v, u);
    }
    dfs(1, 0);
    lca_init(n);
    rp(i,m) {
        scanf("%d%d%d", &u, &v, &k);
        lca_qr[u].push_back(v);
        lca_qr[v].push_back(u);
        lca_ps[u].push_back(i);
        lca_ps[v].push_back(i);
        query[i][0] = u;
        query[i][1] = v;
        query[i][2] = k;
    }
    lca_dfs(1, 0);
    rp(i,m) {
        int o = func_seg_tree.query(root[query[i][0]], root[query[i][1]],
                                    root[lca_ans[i]], root[fa[lca_ans[i]]], query[i][2], 1, p);
        printf("%d\n", sorted[o]);
    }
    return 0;
}
