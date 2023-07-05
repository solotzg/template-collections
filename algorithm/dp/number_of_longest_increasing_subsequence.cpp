// http://www.51nod.com/onlineJudge/questionCode.html#!problemId=1376
#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for (int i(a), __tzg##i(b); i < __tzg##i; ++i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, 1 + b)
#define urp(i, a, b) for (int i(a), __tzg##i(b); i >= __tzg##i; --i)
#define vrp(i, v) for (auto i(v.begin()); i != (v).end(); ++i)
typedef long long ll;
struct Main {
  const ll mod = 1000000007;
  typedef vector<int> VI;
  typedef vector<VI> VVI;
  typedef vector<ll> VL;
  typedef vector<VL> VVL;
  ll solve(const vector<int> &s) {
    int n = s.size();
    VI num;
    VVI cnum;
    VVL sum;
    rp(i, n) {
      auto it = lower_bound(num.begin(), num.end(), s[i]);
      if (it == num.end()) {
        num.push_back(s[i]);
        cnum.push_back(VI());
        sum.push_back(VL(1, 0));
        it = num.end() - 1;
      }
      int k = it - num.begin();
      num[k] = s[i];
      cnum[k].push_back(s[i]);
      ll tp = 0;
      if (k) {
        auto lt = upper_bound(cnum[k - 1].begin(), cnum[k - 1].end(), s[i],
                              greater<int>());
        if (lt == cnum[k - 1].end()) {
          tp = 0;
        } else {
          int l = lt - cnum[k - 1].begin() + 1;
          int r = cnum[k - 1].end() - cnum[k - 1].begin();
          tp = (sum[k - 1][r] - sum[k - 1][l - 1]) % mod;
          if (tp < 0)
            tp += mod;
        }
      } else {
        tp = 1;
      }
      sum[k].push_back((sum[k][sum[k].size() - 1] + tp) % mod);
    }
    return (sum.end() - 1)->back();
  }
};
int main() {
  int n;
  cin >> n;
  vector<int> s(n, 0);
  rp(i, n) cin >> s[i];
  Main a;
  cout << a.solve(s);
  return 0;
}
