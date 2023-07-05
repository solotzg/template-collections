#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
#define vtr(v) (v).begin(), (v).end()
#define mp(a, b) make_pair(a, b)
#define fi first
#define se second
#define pb(a) push_back(a)
#define _0(x) (!(x))
#define _1(x) (x)
typedef vector<int> VI;
typedef pair<int, int> PII;
typedef vector<PII> VPII;
typedef long long ll;
struct BridgeBuilding {
  const static int N = 205;
  const static int inf = 0x3f3f3f3f;
  int minDiameter(VI a, VI b, int K) {
    int p = go(a, b, K);
    return p;
  }
  int go(VI &a, VI &b, int K) {
    int n = a.size() + 1;
    static int sa[N], sb[N], mx[N][N], mxl[N][N], mxr[N][N], ma[N][N],
        tpv[N * 2], dp[N][N];
    mst(sa, 0), mst(sb, 0), mst(ma, 0);
    rp(i, n - 1) sa[i + 1] = sa[i] + a[i], sb[i + 1] = sb[i] + b[i];
    mst(mxl, 0), mst(mxr, 0);
    rp(l, n) rep(r, l + 1, n) {
      mx[l][r] = min(sa[r] - sa[l], sb[r] - sb[l]);
      int ta = sa[r] - sa[l], tb = sb[r] - sb[l];
      repd(k, l, r) {
        mxl[l][r] = max(mxl[l][r], min(sa[k] - sa[l], sa[r] - sa[k] + tb));
        mxl[l][r] = max(mxl[l][r], min(sb[k] - sb[l], sb[r] - sb[k] + ta));
        mxr[l][r] = max(mxr[l][r], min(sa[r] - sa[k], sa[k] - sa[l] + tb));
        mxr[l][r] = max(mxr[l][r], min(sb[r] - sb[k], sb[k] - sb[l] + ta));
      }
      int c = 0, o = 0, sl = 0, sg = 0;
      repd(k, l, r - 1) tpv[c++] = a[k];
      urp(k, r - 1, l) tpv[c++] = b[k];
      rp(i, c) sg += tpv[i];
      rp(i, c) {
        while (sl + tpv[o] <= sg - tpv[o]) {
          sl += tpv[o];
          sg -= tpv[o];
          o = (o + 1) % c;
        }
        ma[l][r] = max(min(sl, sg), ma[l][r]);
        sl -= tpv[i];
        sg += tpv[i];
      }
    }
    {
      int bg = 1, ed = 1e9, tp;
      while (bg < ed) {
        int mid = (bg + ed) / 2;
        mst(dp, inf);
        rep(i, 0, n) {
          dp[i][1] =
              (tp = max(sa[i], sb[i])) > mid || sa[i] + sb[i] > mid ? inf : tp;
        }
        rp(i, n) repd(k, 1, K - 1) if (dp[i][k] != inf) {
          rep(o, i + 1, n) if (ma[i][o] <= mid) {
            if ((tp = dp[i][k] + mxl[i][o]) <= mid) {
              dp[o][k + 1] =
                  min(dp[o][1 + k], max(mxr[i][o], dp[i][k] + mx[i][o]));
            }
          }
        }
        int ans = inf;
        rp(i, n) if (dp[i][K] != inf &&
                     (sa[n - 1] - sa[i] + sb[n - 1] - sb[i]) <= mid &&
                     (tp = dp[i][K] +
                           max(sa[n - 1] - sa[i], sb[n - 1] - sb[i])) <= mid) {
          ans = min(ans, tp);
        }
        if (ans <= mid)
          ed = mid;
        else
          bg = mid + 1;
      }
      return bg;
    }
  }
};
int main() {
  auto a = new BridgeBuilding;
  cout << a->minDiameter(

              {2,  4,  10, 2,  2,  22, 30, 7,  28, 14, 17, 6,  32, 41, 1,
               3,  16, 20, 40, 39, 6,  4,  18, 3,  3,  7,  3,  2,  2,  14,
               10, 6,  38, 6,  13, 3,  6,  1,  23, 26, 9,  17, 1,  38, 1,
               21, 2,  12, 3,  13, 28, 6,  4,  10, 3,  1,  1,  43, 15, 11,
               18, 6,  41, 1,  2,  5,  6,  26, 43, 9,  5,  13, 1,  27, 1,
               33, 12, 16, 1,  12, 1,  37, 34, 6,  20, 3,  21, 3,  1,  17,
               3,  10, 2,  1,  8,  25, 2,  6,  7,  1,  7,  22, 11, 8,  49,
               9,  1,  7,  1,  13, 17, 20, 27, 31, 43, 1},
              {5,  26, 1,  2,  6,  2,  16, 3,  15, 1,  1,  38, 2,  4,  6,
               1,  2,  3,  11, 2,  2,  2,  25, 16, 2,  15, 45, 3,  10, 4,
               17, 5,  6,  1,  16, 13, 29, 3,  1,  2,  4,  47, 4,  10, 1,
               2,  23, 3,  29, 20, 4,  21, 1,  28, 7,  25, 6,  6,  10, 1,
               2,  1,  17, 6,  1,  28, 2,  2,  12, 2,  3,  42, 39, 11, 18,
               3,  15, 4,  1,  15, 3,  9,  4,  26, 4,  13, 41, 1,  27, 1,
               14, 1,  2,  14, 5,  33, 1,  34, 5,  17, 23, 3,  33, 14, 23,
               2,  29, 18, 3,  41, 1,  45, 5,  4,  6,  2},
              5)
       << endl;
  return 0;
}
