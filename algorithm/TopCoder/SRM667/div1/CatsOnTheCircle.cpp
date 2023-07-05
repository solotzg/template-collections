#include <algorithm>
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>
typedef long long int ll;
#define rp(i, b) for (int i = (0), __tzg_##i = (b); i < __tzg_##i; ++i)
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define repd(i, a, b) for (int i = (a), __tzg_##i = (b); i <= __tzg_##i; ++i)
#define mst(a, b) memset(a, b, sizeof(a))
using namespace std;
const double Denominator = 1e9;
const double eps = 1 / Denominator;
struct CatsOnTheCircle {
  double gamblers_ruin(int n, int h, double p) {
    double q = 1.0 - p;
    if (fabs(p - q) < eps)
      return 1.0 * h / n;
    if (q > p)
      return 1 - gamblers_ruin(n, n - h, q);
    double r = q / p;
    return (pow(r, h) - 1) / (pow(r, n) - 1);
  }
  double getProb(int N, int K, int _p) {
    double p = _p / Denominator;
    double q = 1.0 - p;
    double o = gamblers_ruin(N - 2, N - K - 1, p);
    double u = gamblers_ruin(N - 2, K - 1, q);
    return o * gamblers_ruin(N - 1, 1, q) + u * gamblers_ruin(N - 1, 1, p);
  }
};
