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

struct PermutationCounts {
    const static int MOD = 1e9+7;
    inline int mul (int a, int b) {
        return (ll)a*b%MOD;
    }
    inline int add (int a, int b) {
        a += b;
        if (a>=MOD) a-= MOD;
        else if (a < 0) a += MOD;
        return a;
    }
    inline int pow(int a, int b) {
        int r = 1, p = a;
        while (b) {
            if (b & 1) r = mul(r, p);
            p = mul(p, p);
            b >>= 1;
        }
        return r;
    }
    const static int N = 1000005;
    int A[N], VA[N];
    void init() {
        A[0] = 1;
        rep(i,1,N) A[i]=mul(A[i-1], i);
        VA[N-1] = pow(A[N-1], MOD-2);
        urp(i,N-2,0) VA[i]=mul(i+1, VA[i+1]);
    }
    inline int CC(int a, int b) {
        return mul(mul(A[a], VA[b]), VA[a-b]);
    }
    int dp[2600];
    int countPermutations(int n, vector <int> pos) {
        init();
        pos.pb(0);
        pos.pb(n);
        sort(vtr(pos));
        dp[0]=1;
        rep(i, 1, pos.size()) {
            dp[i]=0;
            int f = 1;
            urp(j,i-1,0) {
                dp[i] = add(dp[i], mul(f, mul(dp[j],CC(pos[i],pos[j]))));
                f = -f;
            }
        }
        return dp[pos.size()-1];
    }
};
