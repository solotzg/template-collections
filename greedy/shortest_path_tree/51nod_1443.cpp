#include<map>
#include<cmath>
#include<ctime>
#include<queue>
#include<cstdio>
#include<vector>
#include<bitset>
#include<cstring>
#include<iostream>
#include<algorithm>
using namespace std;
struct Dijkstra {
    typedef long long ll;
    const static int N = 300005;
    const static int M = 300005;
    const ll inf = 0x3f3f3f3f3f3f3f3f;
    struct Edge {
        int to, nxt, val;
        Edge(int _to=-1, int _nxt=-1, int _val=0):to(_to),nxt(_nxt),val(_val) {}
    };
    struct Node {
        int pos;
        ll val;
        int extra;
        Node(int _pos=-1,ll _val=0,int _extra=0):pos(_pos),val(_val),extra(_extra) {}
        bool operator < (const Node & b) const {
            if (val != b.val) return val > b.val;
            return extra > b.extra;
        }
    };
    int n, m, vis[N];
    ll dis[N];
    Edge edge[M*2];
    int head[N], ecnt;
    int start_pt;
    void init() {
        {
            memset(head, -1, sizeof head);
            ecnt = 0;
            memset(dis, inf, sizeof dis);
            memset(vis, 0, sizeof vis);
        }
        cin>>n>>m;
        for (int i = 0; i< m; ++i) {
            int u, v, w;
            scanf("%d%d%d", &u, &v, &w);
            add(u, v, w);
            add(v, u, w);
        }
        cin>>start_pt;
    }
    void add(int u, int v, int w) {
        edge[ecnt] = Edge(v,head[u],w), head[u] = ecnt++;
    }
    void dijkstra(ll & res) {
        res = 0;
        /////////////////
        Node tp;
        ll lp;
        /////////////////
        priority_queue<Node> q;
        q.push(Node(start_pt,0,0));
        dis[start_pt] = 0;
        while(q.empty() == false) {
            tp = q.top();
            q.pop();
            if (vis[tp.pos]) continue;
            vis[tp.pos] = 1;
            res += tp.extra;
            for (int i = head[tp.pos]; i!=-1; i=edge[i].nxt) {
                if (!vis[edge[i].to] && (lp=tp.val+edge[i].val)<=(dis[edge[i].to])) {
                    dis[edge[i].to] = lp;
                    q.push(Node(edge[i].to, dis[edge[i].to], edge[i].val));
                }
            }
        }
    }
} _dijkstra;
typedef long long ll;
int main() {
    ll res;
    _dijkstra.init();
    _dijkstra.dijkstra(res);
    cout<<res<<endl;
    return 0;
}
