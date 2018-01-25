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

struct PolynomialGCD{
    const static int N = 1e4+5, MOD=1e9+7;
    int prime[N], vis[N], np;
    void init_prime(int n) {
        mst(vis, 0);
        np = 0;
        repd(i,2,n) {
            if (!vis[i]) prime[np++] = i;
            for(int j = 0, tp=1; j<np && (tp=prime[j]*i) <= n; ++j) {
                vis[tp]=1;
                if (i % prime[j] == 0) break;
            }
        }
    }
    int cal(int pr, string s) {
        int n = s.size();
        if (n < pr) return 0;
        int res = 1e9;
        rp(bg, pr){
            string o;
            int r = 0;
            for (int pos = bg; pos < n; pos += pr) {
                o += s[pos];
                r += s[pos]-'0';
            }
            res = min(res, r+cal(pr, o));
        }
        return res;
    }
    int gcd(string s){
        int res = 1;
        init_prime(s.size());
        rp(i,np) {
            int p=prime[i], m = cal(p, s);
            rp(_,m) res = (ll)res*p%MOD;
        }
        return res;
    }
};

int main() {
    return 0;
}

