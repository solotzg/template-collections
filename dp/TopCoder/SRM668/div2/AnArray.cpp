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
struct AnArray {
    const static int N = 1000005, M = 250, NA = 2005;
    int mmp[N], da[M];
    int dp[4][NA][M];
    VI A;
    int n;
    int f(int all, int idx, int step) {
        int & res = dp[step][all][idx];
        if (res != -1) return res;
        res = 0;
        int K = da[idx], gcd = __gcd(K,A[all]);
        if (all == 0) {
            if (step == 0) {
                res = K==1?1:0;
            } else if (step == 1) {
                res = gcd==K?1:0;
            } else res = 0;
        } else {
            if (step) {
                res = f(all-1, mmp[K/gcd], step-1) + f(all-1, idx, step);
            } else {
                res = K==1?1:0;
            }
        }
        return res;
    }
    int solveProblem(vector<int> A, int K) {
        int tmp = 0;
        n = A.size();
        this->A = A;
        mst(mmp, -1);
        mst(dp, -1);
        repd(i,1,K) if (K % i == 0) mmp[i] = tmp++, da[tmp-1] = i;
        return f(n-1, mmp[K], 3);
    }
};
int main() {
    auto a = new AnArray;
    assert(a->solveProblem(
               {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},1
                           ) == 220);
    return 0;
}

