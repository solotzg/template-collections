#pragma comment(linker, "/STACK:200000000,200000000")
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stack>
#include <vector>
using namespace std;
struct TreeChainSub {
  const static int N = 100005;
  struct Edge {
    int to, nxt;
    Edge(int _to = -1, int _nxt = -1) : to(_to), nxt(_nxt) {}
  };
  Edge edge[N * 2];
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
  void m_init() {}
  void add(int u, int v) { edge[ecnt] = Edge(v, head[u]), head[u] = ecnt++; }
  void dfs1(int u, int pre) {
    fa[u] = pre;
    dep[u] = pre == -1 ? 1 : (dep[pre] + 1);
    sz[u] = 1;
    son[u] = -1;
    for (int v, i = head[u]; i != -1; i = edge[i].nxt) {
      if ((v = edge[i].to) != pre) {
        dfs1(v, u);
        sz[u] += sz[v];
        if (son[u] == -1 || sz[son[u]] < sz[v])
          son[u] = v;
      }
    }
  }
  void dfs2(int u, int pos) {
    top[u] = pos;
    m_do(u);
    if (son[u] == -1)
      return;
    dfs2(son[u], top[u]);
    for (int v, i = head[u]; i != -1; i = edge[i].nxt) {
      if ((v = edge[i].to) != fa[u] && v != son[u])
        dfs2(v, v);
    }
  }
  void m_do(int u) { pos_tree[u] = ++cn_tree; }
  /*
  void run_example() {
      init();
      dfs1(1, -1);
      dfs2(1, 1);
  }
  */
};
