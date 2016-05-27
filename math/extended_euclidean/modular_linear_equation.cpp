//hihocoder.com/problemset/problem/1303
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
#define bit(x,y) (((x)>>(y))&1)
typedef vector<int> VI;
typedef pair<int,int> PII;
typedef vector<PII> VPII;
typedef long long ll;

const int N = 1005;
ll m[N], r[N];
ll R, M;
ll extend_gcd(ll a,ll b,ll &x,ll &y) {
    if(b==0) {
        x=1;
        y=0;
        return a;
    } else {
        ll r = extend_gcd(b,a%b,x,y);
        ll t=x;
        x=y;
        y=t-(a/b)*y;
        return r;
    }
}
ll solve() {
    int n;
    cin>>n;
    rp(i, n) cin>>m[i]>>r[i];
    M = m[0], R = r[0];
    rep(i, 1, n) {
        ll gcd = __gcd(M, m[i]);
        ll c = -R+r[i];
        if (c % gcd != 0) return -1;
        ll x, y;
        ll r = extend_gcd(M, m[i], x, y);
        ll p = m[i]/gcd;
        ll x0 = x*c/r % p;
        R = R + M * x0;
        M = M/gcd*m[i];
        R %= M;
    }
    if (R < 0) R += M;
    return R;
}

int main() {
#ifdef _TZG_DEBUG
    freopen("in.txt", "r", stdin);
#endif // _TZG_DEBUG
    cout<<solve()<<endl;
    return 0;
}
