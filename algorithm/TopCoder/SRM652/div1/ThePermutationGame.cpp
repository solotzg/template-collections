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

const static int N = 1e5 + 5, MOD = 1e9 + 7;
struct ThePermutationGame {
  int mxPrime[N];
  int m[N];
  void init_prime() {
    memset(mxPrime, 0, sizeof mxPrime);
    for (int i = 2; i < N; ++i) {
      if (mxPrime[i])
        continue;
      for (int j = i; j < N; j += i) {
        mxPrime[j] = i;
      }
    }
  }
  void divide(int x) {
    while (x > 1) {
      int j = mxPrime[x], y = 1;
      while (x % j == 0)
        x /= j, y *= j;
      m[j] = max(m[j], y);
    }
  }
  int findMin(int n) {
    init_prime();
    mst(m, 0);
    int res = 1;
    repd(i, 2, n) { divide(i); }
    repd(i, 2, n) if (m[i]) { res = (ll)res * m[i] % MOD; }
    return res;
  }
};
