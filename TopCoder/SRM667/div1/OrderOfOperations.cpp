#include <cstdio>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstring>
#include <queue>
#include <set>
#include <cmath>
#include <map>
#include <queue>
#include <stack>
using namespace std;
typedef long long ll;
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b); i>=__tzg_##i; --i)
#define rp(i,b) for(int i=(0), __tzg_##i=(b);i<__tzg_##i;++i)
#define rep(i,a,b) for(int i=(a), __tzg_##i=(b);i<__tzg_##i;++i)
#define repd(i,a,b) for(int i=(a), __tzg_##i=(b);i<=__tzg_##i;++i)
#define mst(a,b) memset(a,b,sizeof(a))
typedef pair<int,int> pii;
#define px first
#define py second
const ll mod = 1000000007;
const int MAXN = 4005;
const int MAXM = 15;
const double eps = 1e-6;
#define mp(a,b) make_pair(a,b)
typedef vector<int> VI;
typedef vector<pii> VPII;
#define px first
#define py second
inline int getCnt(int x) {
    int cnt = 0;
    while (x) {
        x &= x-1;
        ++cnt;
    }
    return cnt;
}
struct OrderOfOperations {
    int minTime(vector <string> s) {
        int n = s.size();
        int m = s[0].size();
        int M = 1<<m;
        vector<int> dp(M, -1);
        vector<int> wp(n, 0);
        int op = 0;
        rp(i, n) rp(j, m) {
            if (s[i][j] == '1')
                op |= 1<<j, wp[i] |= (1<<j);
        }
        dp[0] = 0;
        rp(i, op) if (dp[i] != -1) {
            rp(j, n) if ((wp[j] | i) != i) {
                int k = wp[j] | i;
                int o = getCnt(k^i);
                dp[k] = dp[k]==-1?(dp[i]+o*o):(min(dp[k], dp[i]+o*o));
            }
        }
        return dp[op];
    }
};
