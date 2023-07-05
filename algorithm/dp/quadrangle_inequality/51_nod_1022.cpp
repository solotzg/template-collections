/*N��ʯ�Ӱڳ�һ��������Ҫ��ʯ���д���غϲ���һ�ѡ��涨ÿ��ֻ��ѡ���ڵ�2��ʯ�Ӻϲ����µ�һ�ѣ������µ�һ��ʯ������Ϊ�ôκϲ��Ĵ��ۡ����㽫N��ʯ�Ӻϲ���һ�ѵ���С���ۡ�
���磺 1 2 3 4���в��ٺϲ�����
1 2 3 4 => 3 3 4(3) => 6 4(9) => 10(19)
1 2 3 4 => 1 5 4(5) => 1 9(14) => 10(24)
1 2 3 4 => 1 2 7(7) => 3 7(10) => 10(20)
��������Ϊ�ܴ��ۿ��Կ�������һ�ַ����Ĵ�����ͣ����ڸ���n��ʯ�ӵ�������������С�ϲ����ۡ�
*/
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;
typedef long long ll;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
struct Main {
  const static int N = 1005 * 2;
  const int INF = 0x3f3f3f3f;
  int num[N], n, s[N][N], sum[N];
  int dp[N][N];
  void calc() {
    repd(i, 1, n) num[i + n] = num[i];
    repd(i, 1, 2 * n - 1) sum[i] = sum[i - 1] + num[i];
    mst(s, 0);
    mst(dp, 0);
    repd(i, 1, 2 * n - 2) {
      s[i][1 + i] = i;
      dp[i][1 + i] = num[i] + num[i + 1];
    }
    repd(len, 3, n) {
      repd(i, 1, 2 * n - len) {
        int j = i + len - 1;
        dp[i][j] = INF;
        repd(k, s[i][j - 1], s[i + 1][j]) {
          int a = dp[i][k] + dp[k + 1][j] + sum[j] - sum[i - 1];
          if (a < dp[i][j]) {
            dp[i][j] = a;
            s[i][j] = k;
          }
        }
      }
    }
    int res = INF;
    repd(i, 1, n) res = min(res, dp[i][i + n - 1]);
    cout << res << endl;
  }
  void solve() {
    while (scanf("%d", &n) != EOF) {
      repd(i, 1, n) cin >> num[i];
      calc();
    }
  }
} _main;
int main() {
  // freopen("in.txt", "r", stdin);
  _main.solve();
  return 0;
}
