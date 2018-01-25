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

struct LimitedMemorySeries2 {
    ll A, B;
    static const ll U = (1ll<<50), V = U-1ll;
    const static int MOD = 1e9+7;
    inline ll pre(ll x) {
        x |= U;
        x -= B;
        x ^= A;
        return x&V;
    }
    inline ll nxt(ll x) {
        return ((x^A)+B)&V;
    }
    inline void add(int & a, int b) {
        a += b;
        if (a >= MOD) a -= MOD;
        if (a < 0) a += MOD;
    }
    int getSum(int n, ll x, ll a, ll b) {
        A = a;
        B = b;
        int res = 0;
        repd(i,1,n) {
            int ans = 0;
            ll _pre = pre(x), _nxt = nxt(x);
            for (int j = 1; i-j>0&&i+j<=n; ++j) {
                if (x>_pre && x > _nxt) {
                    ans = j;
                    _pre = pre(_pre);
                    _nxt = nxt(_nxt);
                } else break;
            }
            //
            x = nxt(x);
            add(res, ans);
        }
        return res;
    }
};

int main() {
    LimitedMemorySeries2 l;
    cout<<l.getSum(

10000000,
12345678912345ll,
98765094309812ll,
34893049812392ll


                   )<<endl;
    return 0;
}
