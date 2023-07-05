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

struct PolynomialRemainder {
  int cal(int a, int b, int c, int mod) {
    rep(x, 0, mod) {
      ll pa = (ll)a * x % mod * x % mod;
      ll pb = (ll)b * x % mod;
      ll pc = (ll)c % mod;
      if ((pa + pb + pc) % mod == 0)
        return x;
    }
    return -1;
  }
  int findRoot(int a, int b, int c) {
    int p2 = pow(2, 9);
    int p5 = pow(5, 9);
    int x2 = cal(a, b, c, p2);
    int x5 = cal(a, b, c, p5);
    if (x2 == -1 || x5 == -1)
      return -1;
    while (x5 % p2 != x2)
      x5 += p5;
    return x5;
  }
};

int main() {
  PolynomialRemainder p;
  cout << p.findRoot(123456000, 789012345, 678901230) << endl;
  return 0;
}
