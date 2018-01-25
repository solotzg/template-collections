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

struct Nine {
    const static int MAXN = pow(9.0, 5), MOD = 1e9+7;
    int dp[2][MAXN], cnt[5005][9], num[50], dn[10], dm[10];
    int add(int a, int b) {
        return (a+=b)>=MOD?a-MOD:a;
    }
    int mul(int a, int b) {
        return (ll)a*b%MOD;
    }
    int count(int N, vector <int> d) {
        mst(cnt, 0), mst(num, 0), mst(dp, 0);
        rp(i,d.size()) ++num[d[i]];
        cnt[0][0] = 1;
        repd(n,1,d.size()) rp(i,9) repd(j,0,9) cnt[n][(i+j)%9] = add(cnt[n-1][i], cnt[n][(i+j)%9]);
        int cur = 0, nxt = 1-cur;
        dp[cur][0] = 1;
        rp(mask, 1<<N) if (num[mask]) {
            rp(nine, pow(9.0, N)) if(dp[cur][nine]) {
                int x = nine;
                rp(i,N) dn[i] = x%9, x/=9;
                rp(next_num, 9) {
                    rp(pos, N) {
                        if (mask & (1<<pos)) dm[pos] = (dn[pos]+next_num)%9;
                        else dm[pos] = dn[pos];
                    }
                    x = 0;
                    urp(i, N-1, 0) x = x*9 + dm[i];
                    dp[nxt][x] = add(dp[nxt][x], mul(dp[cur][nine], cnt[num[mask]][next_num]));
                }
            }
            swap(cur, nxt);
            mst(dp[nxt], 0);
        }
        return dp[cur][0];
    }
};

int main() {
    auto n = new Nine ;
    cout<<n->count(
            5,
    {1,3,5,8,24,22,25,21,30,2,4,0,6,7,9,11,14,13,12,15,18,17,16,19,26,29,31,28,27,10,20,23}
        )<<endl;
    return 0;
}
