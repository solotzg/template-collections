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
const static int MOD = 1000000007;
struct TwoNumberGroupsEasy {
  int solve(vector<int> A, vector<int> numA, vector<int> B, vector<int> numB) {
    set<int> md;
    md.insert(1);
    md.insert(0);
    int res = accumulate(vtr(numA), 0) + accumulate(vtr(numB), 0);
    VI num;
    unordered_map<int, int> ma;
    int len;
    {
      num.pb(2);
      rp(i, A.size()) rp(j, B.size()) {
        int dif = abs(A[i] - B[j]);
        for (int k = 1; k * k <= dif; ++k)
          if (dif % k == 0)
            num.pb(k), num.pb(dif / k);
      }
      len = num.size();
    }
    auto func = [&](int mod) {
      if (md.find(mod) != md.end())
        return;
      md.insert(mod);
      ma.clear();
      rp(i, A.size()) { ma[A[i] % mod] += numA[i]; }
      rp(i, B.size()) { ma[B[i] % mod] -= numB[i]; }
      int sum = 0;
      for (auto &e : ma) {
        sum += abs(e.se);
      }
      res = min(res, sum);
    };
    rp(i, len) func(num[i]);
    return res;
  }
};

int main() {
#ifdef _TZG_DEBUG
  freopen("in.txt", "r", stdin);
#endif // __TZG_DEBUG
  TwoNumberGroupsEasy t;
  cout << t.solve({113566284, 186977467, 101966682, 880561409, 847678114,
                   395611414, 99288152, 366683128, 539755250, 733739043},
                  {5046, 5178, 5011, 4764, 3502, 6390, 4119, 3492, 8208, 7574},
                  {319521701, 395611414, 982900878, 994500480, 733739043,
                   28555499, 25876969, 953972592, 554033382, 539755250},
                  {6916, 3315, 5392, 8167, 8270, 2697, 3041, 7557, 5555, 2371})
       << endl;
  return 0;
}
