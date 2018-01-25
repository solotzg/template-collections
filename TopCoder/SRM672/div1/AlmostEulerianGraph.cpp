#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
typedef long long ll;
struct AlmostEulerianGraph{
    const static int N = 2005;
    const ll mod = 1000000007;
    ll pow(ll a, ll b) {
        ll r = 1, p =a;
        while (b) {
            if (b & 1) r = r*p%mod;
            p = p*p %mod;
            b >>= 1;
        }
        return r;
    }
    ll comb[N][N];
    ll dp[N], p2[N];
    void init(int n) {
        repd(i,0,n) {
            comb[i][0] = comb[i][i] = 1;
            rep(j, 1, i) comb[i][j] = (comb[i-1][j]+comb[i-1][j-1])%mod;
        }
    }
    ll calc(int n) {
        return pow(2, (n-1)*(n-2)/2);
    }
    int calculateGraphs(int n){
        init(n);
        dp[1] = 1;
        p2[0] = 1, p2[1] = 1;
        repd(i, 2, n) {
            ll tp = calc(i);
            ll ap = 0;
            p2[i] = tp;
            rep(j, 1, i) {
                ap += comb[i-1][j-1]*dp[j]%mod*p2[i-j]%mod;
                if (ap >= mod) ap -= mod;
            }
            tp -= ap;
            if (tp < 0) tp += mod;
            dp[i] = tp;
        }
        return dp[n]*(1+comb[n][2])%mod;
    }
};
int main() {
    return 0;
}
