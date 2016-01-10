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
const double eps = 1e-6;
#define mp(a,b) make_pair(a,b)
typedef vector<int> VI;
#define px first
#define py second
ll C[MAXN][MAXN];
ll dp[4][MAXN];
int N;
struct SumOverPermutations{
    int findSum(int n);
    SumOverPermutations(){
        mst(C, 0);
        rp(i,MAXN){
            C[i][0] = 1;
            rep(j,1,i+1)
                C[i][j] = (C[i-1][j-1]+C[i-1][j])%mod;
        }
    }
};
ll dfs(int n, int ty) {
    if (dp[ty][n] != -1)
        return dp[ty][n];
    ll & res = dp[ty][n];
    if (n == 0)
        res = 1;
    else {
        ll sum = 0;
        rp(i, n) {
            ll x = N;
            if (i == 0 && (ty & 2)) --x;
            if (i == n-1 && (ty & 1)) --x;
            ll r1 = dfs(i, ty | 1);
            ll r2 = dfs(n-1-i, ty | 2);
            ll p = r1*r2%mod*x%mod*C[n-1][i]%mod;
            sum = (sum+p)%mod;
        }
        res = sum;
    }
    return res;
}
int SumOverPermutations::findSum(int n){
    mst(dp, -1);
    N = n;
    return (int) dfs(n, 0);
}
