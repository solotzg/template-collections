#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
#define vtr(v) (v).begin(),(v).end()
#define mp(a,b) make_pair(a,b)
#define fi first
#define se second
#define pb(a) push_back(a)
#define _0(x) (!(x))
#define _1(x) (x)
typedef vector<int> VI;
typedef pair<int,int> PII;
typedef vector<PII> VPII;
typedef long long ll;

struct SuccessiveSubtraction2{
    const static int N = 2005, INF = 0x3f3f3f3f;
    int dp[N][5];
    VI calc(VI a, VI p, VI v) {
        int n = a.size(), q = p.size();
        VI res(q, -INF);
        rp(i, q) {
            a[p[i]] = v[i];
            rp(j,n)rp(k,5) dp[j][k]=-INF;
            dp[0][0] = a[0];
            dp[1][0] = a[0]-a[1];
            rep(j,2,n) {
                int f = 1;
                rp(k,5) {
                    f = -f;
                    int & u = dp[j][k];
                    if (k && dp[j-1][k-1]!=-INF)
                        u = max(u, dp[j-1][k-1]+f*a[j]);
                    if (dp[j-1][k] != -INF)
                        u = max(u, dp[j-1][k]+f*a[j]);
                }
            }
            rp(j,5) res[i] = max(res[i], dp[n-1][j]);
        }
        return res;
    }
};

int main() {
    SuccessiveSubtraction2 * s = new SuccessiveSubtraction2;
    VI v = s->calc(


{-11, -4, 28, 38, 21, -29, -45, 11, -58, -39, 92, 35, -56, -6, 29, -2, 61, 10, -29, -63},
{19, 5, 3, 10, 4, 18, 5, 2, 0, 15},
{-19, 21, 7, -66, 38, -39, -22, 24, -32, 13}
                   );
    for (int o : v) {
        cout<<o<<endl;
    }
    return 0;
}
