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

struct Nine{
    const static int MOD = 1e9+7, N = pow(9.0, 5.0)+5;
    int dp[2][N], cnt[5005][10], s[50];
    inline int add(int a, int b) {
        a += b;
        if (a >= MOD) a -= MOD;
        else if (a < 0) a += MOD;
        return a;
    }
    inline int mul(int a, int b) {
        return (ll)a*b%MOD;
    }
    int a[10], b[10];
    int count(int n, VI d){
        mst(s, 0);
        rp(i,d.size())
            s[d[i]]++;
        mst(cnt, 0);
        cnt[0][0] = 1;
        rep(i,0,5004)rp(j,9)rp(k,10) {
            cnt[i+1][(j+k)%9] = add(cnt[i+1][(j+k)%9], cnt[i][j]);
        }
        int cur = 0, nxt = 1-cur;
        mst(dp, 0);
        dp[cur][0] = 1;
        rp(i,50) if (s[i]) {
            int * c = cnt[s[i]];
            rp(mask, pow(9.0, n)) if (dp[cur][mask]) {
                rp(o,9) {
                    int x = mask, nmask = 0;
                    rp(j,n) b[j]=x%9, x/=9;
                    rp(j,n) {
                        if ((i>>j) & 1)
                            b[j]=(b[j]+o)%9;
                    }
                    urp(j,n-1,0) nmask = nmask*9+b[j];
                    dp[nxt][nmask] = add(dp[nxt][nmask], mul(dp[cur][mask], c[o]));
                }
            }
            ///
            swap(cur, nxt);
            mst(dp[nxt], 0);
        }
        return dp[cur][0];
    }
};

int main() {
    auto n = new Nine ;
    cout<<n->count(
                   2,{1,2,3}
                   )<<endl;
    return 0;
}
