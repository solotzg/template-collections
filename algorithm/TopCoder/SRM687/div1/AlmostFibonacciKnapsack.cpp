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

int b[200];
struct AlmostFibonacciKnapsack {
  vector<ll> nums;
  int lb;
  int dfs(int idx, ll rm) {
    if (idx < 1) {
      return rm == 0;
    }
    if (rm == 0)
      return 1;
    urp(i, idx, 1) {
      ll o = nums[i];
      if (o <= rm) {
        b[lb++] = i;
        if (dfs(i - 1, rm - o))
          return 1;
        --lb;
      }
    }
    return 0;
  }
  vector<int> getIndices(long long x) {
    nums.push_back(0);
    nums.push_back(2);
    nums.push_back(3);
    for (int i = 3;; ++i) {
      ll a = nums[i - 1] + nums[i - 2] - 1ll;
      if (a > x)
        break;
      nums.push_back(a);
    }
    lb = 0;
    if (dfs(nums.size() - 1, x) == 0)
      return {-1};
    VI res;
    rp(i, lb) res.pb(b[i]);
    return res;
  }
};
