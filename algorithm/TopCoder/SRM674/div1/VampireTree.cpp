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

struct VampireTree {
  int maxDistance(vector<int> num) {
    int one = 0, sum = 0;
    for (int i : num) {
      one += i == 1;
      sum += i == 1 ? 0 : i - 2;
    }
    sum += 2;
    if (sum != one)
      return -1;
    return num.size() - one + 1;
  }
};

int main() {
  VampireTree *v = new VampireTree;
  assert(v->maxDistance({3, 1, 1, 3, 1, 2, 1}) == 4);
  assert(v->maxDistance(
             {2, 3, 3, 4, 1, 2, 3, 1, 1, 2, 2, 1, 1, 3, 1, 3, 1, 1, 1}) == 11);
  return 0;
}
