#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
using namespace std;
const int MAXN = 60;
const int MAXM = MAXN * MAXN;
int n, m;
int mmp[MAXN][MAXN], vis[MAXN];
struct DLX {
  int U[MAXM], D[MAXM], L[MAXM], R[MAXM], C[MAXM];
  int H[MAXN], S[MAXN];
  int sz, ans;
  void init() {
    memset(mmp, 0, sizeof mmp);
    memset(H, -1, sizeof H);
    for (int i = 0; i <= n; ++i) {
      S[i] = 0;
      U[i] = D[i] = i;
      L[i + 1] = i;
      R[i] = i + 1;
    }
    R[n] = 0;
    sz = n + 1;
  }
  void link(int r, int c) {
    ++S[c];
    C[sz] = c;
    U[sz] = U[c];
    D[sz] = c;
    U[c] = sz;
    D[U[sz]] = sz;
    if (H[r] == -1)
      H[r] = L[sz] = R[sz] = sz;
    else {
      L[sz] = L[H[r]];
      R[sz] = H[r];
      R[L[sz]] = sz;
      L[R[sz]] = sz;
    }
    ++sz;
  }
  void remv(int dx) {
    for (int i = D[dx]; i != dx; i = D[i])
      L[R[i]] = L[i], R[L[i]] = R[i];
  }
  void resum(int dx) {
    for (int i = D[dx]; i != dx; i = D[i])
      L[R[i]] = R[L[i]] = i;
  }
  int h() {
    int cnt = 0;
    memset(vis, 0, sizeof vis);
    for (int i = R[0]; i; i = R[i]) {
      if (vis[i])
        continue;
      ++cnt;
      for (int j = D[i]; j != i; j = D[j]) {
        for (int k = R[j]; k != j; k = R[k])
          vis[C[k]] = 1;
      }
    }
    return cnt;
  }
  void dance(int k) {
    int mn, dx;
    if (k + h() >= ans)
      return;
    if (!R[0]) {
      if (k < ans)
        ans = k;
      return;
    }
    mn = MAXN;
    for (int i = R[0]; i; i = R[i])
      if (mn > S[i])
        mn = S[i], dx = i;
    for (int i = D[dx]; i != dx; i = D[i]) {
      remv(i);
      for (int j = R[i]; j != i; j = R[j])
        remv(j);
      dance(k + 1);
      for (int j = R[i]; j != i; j = R[j])
        resum(j);
      resum(i);
    }
  }
  int cal() {
    for (int i = 1; i <= n; ++i) {
      for (int j = 1; j <= n; ++j) {
        if (mmp[i][j] || i == j)
          link(i, j);
      }
    }
    ans = n;
    dance(0);
    return ans;
  }
} node;
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
  // freopen("out.txt", "w", stdout);
#endif
  while (scanf("%d%d", &n, &m) != EOF) {
    node.init();
    for (int i = 0, a, b; i < m; ++i) {
      scanf("%d%d", &a, &b);
      mmp[a][b] = mmp[b][a] = 1;
    }
    printf("%d\n", node.cal());
  }
  return 0;
}
