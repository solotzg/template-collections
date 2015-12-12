#include <stdio.h>
#define rep(i,a,b) for(int i(a), __tzg##i(b);i<__tzg##i;++i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,1+b)
#define urp(i,a,b) for(int i(a), __tzg##i(b);i>=__tzg##i;--i)
#define vrp(i,v) for(auto i(v.begin());i!=(v).end();++i)
struct Main {
    typedef long long ll;
    const static int N = 100005;
    ll pow(ll a, ll b) {
        ll r = 1, p = a;
        while (b) {
            if (b & 1) r = r*p%mod;
            p = p*p%mod;
            b >>= 1;
        }
        return r;
    }
    ll mod, q;
    int K,Q;
    ll a[N], b[N], o[N];
    ll cal0(int n, ll l1[], ll l2[]) {
        ll res = 0;
        rp(i, n)res = (res + l1[i]*l2[i]%mod)%mod;
        return res;
    }
    ll cal1(ll l[], ll g, int pos) {
        return l[pos]*g%mod;
    }
    Main() {
        rp(i,N) o[i] = i;
    }
    void solve() {
        scanf("%d%d%I64d", &K,&Q,&mod);
        rp(i,K) scanf("%I64d", a+i);
        rp(i,K) scanf("%I64d", b+i);
        ll obb = cal0(K, a, b);
        ll oll = cal0(K, o, b);
        while (Q--) {
            scanf("%I64d", &q);
            ll n = q/K;
            if (!n) {
                printf("%I64d\n", q%mod);
                continue;
            }
            ll ob = pow(obb, n-1);
            ll ol = oll*ob%mod;
            printf("%I64d\n", cal1(a,ol,q%K));
        }
    }
} _main;
int main() {
    //freopen("in.txt", "r", stdin);
    int t;
    scanf("%d", &t);
    while (t--) {
        _main.solve();
    }
    return 0;
}
