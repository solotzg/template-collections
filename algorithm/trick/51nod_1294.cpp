#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
const int N = 100005;
int a[N], b[N], c, d[N];
int main() {
  int n;
  cin >> n;
  rp(i, n) cin >> a[i];
  c = 0;
  rp(i, n) {
    int k = a[i] - (i + 1);
    if (k >= 0) {
      b[c++] = k;
    }
  }
  int r = 0;
  int nd = 0;
  rp(i, c) {
    int ps = upper_bound(d, d + nd, b[i]) - d;
    if (ps == nd)
      ++nd;
    d[ps] = b[i];
  }
  cout << n - nd << endl;
  return 0;
}
