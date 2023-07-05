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
struct BearAttacks {
  const static int N = 1000005, MOD = 1e9 + 7;
  VI e[N];
  int pw(int a, int b) {
    int r = 1, p = a;
    while (b) {
      if (b & 1)
        r = mul(r, p);
      p = mul(p, p);
      b >>= 1;
    }
    return r;
  }
  inline int add(int a, int b) { return (a += b) >= MOD ? a - MOD : a; }
  inline int mul(int a, int b) { return (ll)a * b % MOD; }
  int expectedValue(int N, int R0, int A, int B, int M, int LOW, int HIGH) {
    int R = R0;
    int BILLION = 1000000000;
    repd(i, 1, N - 1) {
      R = ((ll)A * R + B) % M;
      int MIN = ((ll)(i - 1) * LOW) / BILLION;
      int MAX = ((ll)(i - 1) * HIGH) / BILLION;
      int j = MIN + R % (MAX - MIN + 1);
      e[i].pb(j);
      e[j].pb(i);
    }
    int n = 0, res = 0;
    VI s(N), inv(N), fa(N), d1(N);
    queue<int> q;
    q.push(0);
    s[n++] = 0;
    fa[0] = -1;
    while (q.empty() == false) {
      int u = q.front();
      q.pop();
      for (int v : e[u])
        if (v != fa[u]) {
          q.push(v);
          s[n++] = v;
          fa[v] = u;
        }
    }
    rp(i, N) inv[i] = pw(i + 1, MOD - 2);
    urp(i, n - 1, 0) {
      int u = s[i];
      int p = 1;
      for (int v : e[u])
        if (v != fa[u]) {
          res = add(res, mul(mul(p, inv[u]), d1[v]));
          p = add(p, d1[v]);
        }
      d1[u] = mul(p, inv[u]);
    }
    res = mul(res, 2);
    rp(i, N) res = add(res, inv[i]);
    repd(i, 2, N) res = (ll)res * i % MOD;
    return res;
  }
};
int main() {
  auto a = new BearAttacks;
  // assert(a->expectedValue(3,0,2,3,100,938593850,1000000000) == 21);
  // assert(a->expectedValue(3,0,0,0,1,0,0) == 23);
  assert(a->expectedValue(6, 303840291, 848660283, 395739574, 950123456, 0,
                          1000000000) == 3856);
  return 0;
}
