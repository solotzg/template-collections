#include <bits/stdc++.h>
using namespace std;
struct Coversta {
  const static int N = 105;
  int check[N][N], mmp[N][N], sum[N][N], vis[N][N];
  int r, c;
  inline int ok(int x, int y) { return x >= 0 && x < r && y >= 0 && y < c; }
  int place(vector<string> a, vector<int> x, vector<int> y) {
    r = a.size();
    c = a[0].size();
    int o = x.size();
    for (int i = 0; i < r; ++i) {
      for (int j = 0; j < c; ++j) {
        mmp[i][j] = a[i][j] - '0';
        sum[i][j] = 0;
      }
    }
    for (int i = 0; i < r; ++i) {
      for (int j = 0; j < c; ++j)
        for (int k = 0; k < o; ++k) {
          int xx = i + x[k], yy = j + y[k];
          if (ok(xx, yy))
            sum[i][j] += mmp[xx][yy];
        }
    }
    int cnt = 0;
    memset(check, 0, sizeof check);
    memset(vis, 0, sizeof vis);
    int res = 0;
    for (int i = 0; i < r; ++i) {
      for (int j = 0; j < c; ++j) {
        ++cnt;
        for (int k1 = 0; k1 < o; ++k1) {
          int xx, yy;
          xx = i + x[k1], yy = j + y[k1];
          if (ok(xx, yy))
            vis[xx][yy] = cnt;
          for (int k2 = 0; k2 < o; ++k2) {
            xx = i + x[k1] - x[k2], yy = j + y[k1] - y[k2];
            if (ok(xx, yy))
              check[xx][yy] = cnt;
          }
        }
        for (int ii = i; ii < r; ++ii) {
          for (int jj = 0; jj < c; ++jj)
            if (ii > i || jj > j) {
              if (check[ii][jj] != cnt) {
                res = max(res, sum[i][j] + sum[ii][jj]);
              } else {
                int s = 0;
                for (int k = 0; k < o; ++k) {
                  int xx = ii + x[k], yy = jj + y[k];
                  if (ok(xx, yy)) {
                    if (vis[xx][yy] != cnt)
                      s += mmp[xx][yy];
                  }
                }
                res = max(res, sum[i][j] + s);
              }
            }
        }
      }
    }
    return res;
  }
};
