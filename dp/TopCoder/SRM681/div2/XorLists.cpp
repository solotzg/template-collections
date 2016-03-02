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

struct XorLists {
    const static int N = 12, MOD=1e9+7;
    void add(int & a, int b){
        a += b;
        a -= a>=MOD?MOD:0;
    }
    int dp[35][1<<N];
    int mmp[N][N], op[N][N], mm[35];
    int bts[1<<N][N];
    int countLists(vector<int> s,int m) {
        int n = sqrt(s.size());
        rp(i,n)rp(j,n) mmp[i][j] = s[i*n+j];
        rp(i,n) {
            if (mmp[i][i]!=0) return 0;
            rep(j,1,n) if (mmp[i][j]!=mmp[j][i]) return 0;
        }
        rp(i,30) mm[i]=(m>>i)&1;
        rp(mask,1<<n) rp(j,n) bts[mask][j]=(mask>>j)&1;
        mst(dp, 0);
        dp[30][(1<<n)-1]=1;
        urp(pos, 29, 0) {
            rp(i,n)rp(j,n) op[i][j]=(mmp[i][j]>>pos)&1;
            rp(cur, 1<<n) {
                int * p = bts[cur];
                int f = 1;
                for (int i = 0; i< n && f; ++i) for (int j = i+1; j<n && f; ++j) if ((p[i]^p[j])!=op[i][j]) f = 0;
                if (f) {
                    rp(last, 1<<n) {
                        int * q = bts[last];
                        f = 1;
                        int sn = 0;
                        rp(i,n) {
                            if (q[i] && p[i]>mm[pos]) {
                                f = 0;
                                break;
                            }
                            if (q[i] && p[i]==mm[pos]) sn |= 1<<i;
                        }
                        if (f)
                            add(dp[pos][sn], dp[pos+1][last]);
                    }
                }
            }
        }
        int res = 0;
        rp(i,1<<n) add(res, dp[0][i]);
        return res;
    }
};

int main() {
    XorLists x;
    cout<<x.countLists(
                       {0,1,2, 1,0,3, 2,3,0},5
                       )<<endl;
    return 0;
}

