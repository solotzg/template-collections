#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
typedef long long ll;
struct BearPermutations {
  const static int N = 105;
  int f[N][N][N], g1[N][N], g2[N][N];
  int comb[N][N];
  int mod;
  int mul(int a, int b) { return (ll)a * b % mod; }
  int add(int a, int b) { return a += b, a >= mod ? a - mod : a; }
  int countPermutations(int n, int s, int mod) {
    this->mod = mod;
    rp(i, N) {
      comb[i][0] = comb[i][i] = 1;
      rep(j, 1, i) { comb[i][j] = add(comb[i - 1][j - 1], comb[i - 1][j]); }
    }
    mst(f, 0);
    mst(g1, 0);
    mst(g2, 0);
    f[1][0][0] = 1;
    g1[1][1] = 1;
    g2[1][1] = 1;
    repd(i, 2, n) {
      rp(j, i) {
        int a = j, b = i - 1 - a;
        repd(k, 0, s) {
          int &ff = f[i][j][k];
          ff = 0;
          if (a > b)
            ff = f[i][i - 1 - j][k];
          else if (a == 0) {
            rp(o, b) { ff = add(ff, f[b][o][k]); }
          } else if (b == 0) {
            rp(o, a) { ff = add(ff, f[a][o][k]); }
          } else {
            repd(s1, 0, k) {
              ff = add(ff, mul(g1[a][s1], mul(g2[b][k - s1], comb[i - 1][a])));
            }
          }
          if (j + k + 1 <= s)
            g2[i][1 + j + k] = add(g2[i][1 + j + k], ff);
          if (k + i - j <= s)
            g1[i][k + i - j] = add(g1[i][k + i - j], ff);
        }
      }
    }
    int res = 0;
    repd(i, 0, s) rp(j, n) res = add(res, f[n][j][i]);
    return res;
  }
};
int main() {
  BearPermutations *b = new BearPermutations;
  int k = b->countPermutations(100, 100, 215155649);
  cout << k << endl;
  return 0;
}
