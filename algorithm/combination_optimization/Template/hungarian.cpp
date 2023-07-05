int da[N][N];
int belong[N], vis[N];
int dfs(int u) {
  rp(v, n) if (da[u][v] && !vis[v]) {
    vis[v] = 1;
    if (belong[v] == -1 || dfs(belong[v])) {
      belong[v] = u;
      return 1;
    }
  }
  return 0;
}
int hungarian() {
  mst(belong, -1);
  int cnt = 0;
  rp(u, n) {
    mst(vis, 0);
    cnt += dfs(u);
  }
  return cnt;
}
