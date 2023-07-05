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
#define bit(x, y) (((x) >> (y)) & 1)
typedef vector<int> VI;
typedef pair<int, int> PII;
typedef vector<PII> VPII;
typedef long long ll;

struct BoardFolding {
  int tonumber(char c) {
    if (c >= '0' && c <= '9')
      return c - '0';
    if (c >= 'a' && c <= 'z')
      return 10 + c - 'a';
    if (c >= 'A' && c <= 'Z')
      return 36 + c - 'A';
    return c == '#' ? 62 : 63;
  }
  int howMany(int N, int M, vector<string> compressedPaper) {
    vector<VI> paper(N, VI(M));
    rp(i, N) rp(j, M) paper[i][j] =
        (tonumber(compressedPaper[i][j / 6]) >> (j % 6)) % 2;
    int aw = cal(paper);
    vector<VI> r(M, VI(N));
    rp(i, M) rp(j, N) r[i][j] = paper[j][i];
    int ah = cal(r);
    return aw * ah;
  }
  int cal(vector<VI> &paper) {
    int r = paper.size();
    int c = paper[0].size();
    VI bord(r, 0);
    rp(center, r - 1) {
      repd(len, 1, 1e9) {
        if (center - len + 1 < 0 || center + len > r - 1)
          break;
        int f = 1;
        rp(j, c) {
          if (paper[center - len + 1][j] != paper[center + len][j]) {
            f = 0;
            break;
          }
        }
        if (!f)
          break;
        bord[center] = len;
      }
    }
    vector<VI> dp(r, VI(r, 0));
    dp[0][r - 1] = 1;
    rp(i, r) urp(j, r - 1, i + 1) if (dp[i][j]) {
      rep(center, i, j) {
        if (center - bord[center] + 1 <= i &&
            center - i + 1 <= j - (center + 1) + 1)
          dp[center + 1][j] = 1;
        if (center + bord[center] >= j &&
            center - i + 1 >= j - (center + 1) + 1)
          dp[i][center] = 1;
      }
    }
    int res = 0;
    rp(i, r) rp(j, r) res += dp[i][j];
    return res;
  }
};

int main() {
  BoardFolding b;
  printf(
      "%d\n",
      b.howMany(64, 64,
                {"mCmqmCCBmC6", "FpFBFppqFp9", "FpFBFppqFp9", "mCmqmCCBmC6",
                 "FpFBFppqFp9", "mCmqmCCBmC6", "mCmqmCCBmC6", "FpFBFppqFp9",
                 "FpFBFppqFp9", "mCmqmCCBmC6", "mCmqmCCBmC6", "FpFBFppqFp9",
                 "mCmqmCCBmC6", "FpFBFppqFp9", "FpFBFppqFp9", "mCmqmCCBmC6",
                 "FpFBFppqFp9", "mCmqmCCBmC6", "mCmqmCCBmC6", "FpFBFppqFp9",
                 "mCmqmCCBmC6", "FpFBFppqFp9", "FpFBFppqFp9", "mCmqmCCBmC6",
                 "mCmqmCCBmC6", "FpFBFppqFp9", "FpFBFppqFp9", "mCmqmCCBmC6",
                 "FpFBFppqFp9", "mCmqmCCBmC6", "mCmqmCCBmC6", "FpFBFppqFp9",
                 "FpFBFppqFp9", "mCmqmCCBmC6", "mCmqmCCBmC6", "FpFBFppqFp9",
                 "mCmqmCCBmC6", "FpFBFppqFp9", "FpFBFppqFp9", "mCmqmCCBmC6",
                 "mCmqmCCBmC6", "FpFBFppqFp9", "FpFBFppqFp9", "mCmqmCCBmC6",
                 "FpFBFppqFp9", "mCmqmCCBmC6", "mCmqmCCBmC6", "FpFBFppqFp9",
                 "mCmqmCCBmC6", "FpFBFppqFp9", "FpFBFppqFp9", "mCmqmCCBmC6",
                 "FpFBFppqFp9", "mCmqmCCBmC6", "mCmqmCCBmC6", "FpFBFppqFp9",
                 "FpFBFppqFp9", "mCmqmCCBmC6", "mCmqmCCBmC6", "FpFBFppqFp9",
                 "mCmqmCCBmC6", "FpFBFppqFp9", "FpFBFppqFp9", "mCmqmCCBmC6"}));
  return 0;
}
