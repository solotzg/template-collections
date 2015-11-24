#pragma comment(linker, "/STACK:200000000,200000000")
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stack>
using namespace std;
int pos_tree[100005], cn_tree;
long long res[100005];
struct TreeChainSub {
    const static int N = 100005;
    struct Edge {
        int to, nxt;
        Edge(int _to=-1, int _nxt=-1):to(_to),nxt(_nxt) {}
    };
    Edge edge[N*2];
    /*
        节点标号从1开始
    */
    int head[N], ecnt;
    int son[N], sz[N], top[N], fa[N], dep[N];
    void init() {
        memset(head, -1, sizeof head);
        ecnt = 0;
        top[0] = 0, sz[0] = 0, son[0] = 0, fa[0] = 0, dep[0] = 0;
        m_init();
    }
    void m_init() {
    }
    void add(int u, int v) {
        edge[ecnt] = Edge(v,head[u]), head[u] = ecnt++;
    }
    // 我草你妈的，居然爆栈了。。。。。
    void dfs1(int u, int pre) {
        fa[u] = pre;
        dep[u] = pre==-1?1:(dep[pre]+1);
        sz[u] = 1;
        son[u] = -1;
        for (int v, i = head[u]; i!=-1; i=edge[i].nxt) {
            if ((v = edge[i].to) != pre) {
                dfs1(v, u);
                sz[u] += sz[v];
                if (son[u] == -1 || sz[son[u]] < sz[v])
                    son[u] = v;
            }
        }
    }
    void bfs1(int bg) {
        static int chd[N], state[N];
        memcpy(chd, head, sizeof head);
        memset(state, 0, sizeof state);
        stack<pair<int,int> > q;
        pair<int,int> tp;
        int u, pre, v, r;
        q.push(make_pair(bg, -1));
        while (!q.empty()) {
            tp = q.top();
            u = tp.first, pre = tp.second, r = chd[u];
            if (!state[u]) {
                state[u] = 1;
                fa[u] = pre;
                dep[u] = pre==-1?1:(dep[pre]+1);
                sz[u] = 1;
                son[u] = -1;
            }
            if (r == -1) {
                if (pre != -1) {
                    sz[pre] += sz[u];
                    if (son[pre] == -1 || sz[son[pre]] < sz[u])
                        son[pre] = u;
                }
                q.pop();
            } else {
                v = edge[r].to;
                if (v != pre) q.push(make_pair(v, u));
                chd[u] = edge[r].nxt;
            }
        }
    }
    void dfs2(int u, int pos) {
        top[u] = pos;
        m_do(u);
        if (son[u] == -1) return;
        dfs2(son[u], top[u]);
        for (int v, i = head[u]; i!=-1; i=edge[i].nxt) {
            if ((v=edge[i].to) != fa[u] && v!=son[u])
                dfs2(v, v);
        }
    }
    void bfs2(int u, int pos) {
        static int chd[N], state[N];
        memcpy(chd, head, sizeof head);
        memset(state, 0, sizeof state);
        stack<pair<int,int> > q;
        pair<int,int> tp;
        q.push(make_pair(u, pos));
        while (q.empty() == false) {
            tp = q.top();
            u = tp.first, pos = tp.second;
            int r = chd[u];
            if (!state[u]) {
                state[u] = 1;
                top[u] = pos;
                m_do(u);
                if (son[u] != -1) {
                    q.push(make_pair(son[u], top[u]));
                } else {
                    q.pop();
                }
                continue;
            }
            if (r == -1) {
                q.pop();
            } else {
                int v = edge[r].to;
                if (v != son[u] && v != fa[u]) q.push(make_pair(v,v));
                chd[u] = edge[r].nxt;
            }
        }
    }
    void m_do(int u) {
        pos_tree[u] = ++cn_tree;
    }
    /*
    void run_example() {
        init();
        dfs1(1, 0);
        dfs2(1, 1);
    }
    */
};
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
        ll vt, vv, vd;
        Node(ll _vt = 0, ll _vv = 0, ll _vd = 0):vt(_vt),vv(_vv),vd(_vd) {}
    };
    Node da[N*3];
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
        da[lr].vt += da[root].vt + da[lr].vv*da[root].vd;
        da[lr].vd += da[root].vd;
        da[lr].vv += da[root].vv;

        da[rr].vt += da[root].vt + da[rr].vv*da[root].vd;
        da[rr].vd += da[root].vd;
        da[rr].vv += da[root].vv;

        da[root] = Node();
    }
    void update(int L, int R, ll d, int f, int l, int r, int root) {
        if (L <= l && r <= R) {
            if (f == 1) {
                da[root].vv += d;
            } else {
                da[root].vt += da[root].vv*d;
                da[root].vd += d;
            }
            return;
        }
        merge_down(l,r,root);
        int m = (l+r)/2;
        if (m >= L) update(L,R,d,f,lson);
        if (m <  R) update(L,R,d,f,rson);
    }
    void update_all(int l, int r, int root) {
        if (l == r) {
            res[l] = da[root].vt;
            return;
        }
        merge_down(l,r,root);
        int m = (l+r)/2;
        update_all(lson);
        update_all(rson);
    }
};
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
typedef long long ll;
TreeChainSub sb;
SegmentTree st;
int n, q, o, u, d;
void build_tree(int a, int b) {
    int *top = sb.top;
    int *dep = sb.dep;
    int *fa = sb.fa;
    while (top[a] != top[b]) {
        if (dep[top[a]] < dep[top[b]]) swap(a, b);
        st.update(pos_tree[top[a]], pos_tree[a], d, o, 1, n, 1);
        a = fa[top[a]];
    }
    if (dep[a] > dep[b]) swap(a, b);
    st.update(pos_tree[a], pos_tree[b], d, o, 1, n, 1);
}
int main() {
    //freopen("in.txt", "r", stdin);
    sb.init();
    cn_tree = 0;
    cin>>n;
    repd(i, 2, n) {
        int k;
        scanf("%d", &k);
        sb.add(k, i);
    }
    sb.dfs1(1,-1);
    sb.dfs2(1,1);
    st.build(1, n, 1);
    cin>>q;
    while(q--) {
        scanf("%d%d%d", &o, &u, &d);
        build_tree(1, u);
    }
    st.update_all(1,n,1);
    repd(i,1,n) {
        printf("%lld\n", res[pos_tree[i]]);
    }
    return 0;
}
