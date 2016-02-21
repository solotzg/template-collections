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

const int N = 1e5+5;
inline ll sqr(int x) {return (ll)x*x;}
int n;
PII pt[N];
int bgmax[N], edmax[N], bgmin[N], edmin[N];
ll cal() {
    sort(pt, pt+n);
    mst(bgmax, 0); mst(edmax, 0); mst(bgmin, 0); mst(edmin, 0);
    rp(i, n) bgmax[i+1] = max(bgmax[i], pt[i].se), bgmin[i+1]=min(bgmin[i], pt[i].se);
    urp(i, n-1, 0) edmax[i] = max(edmax[i+1], pt[i].se), edmin[i]=min(edmin[i+1], pt[i].se);
    ll bg = 0, ed = 1e17;
    while (bg < ed) {
        ll mid = (bg + ed)/2;
        int f = sqr(pt[n-1].fi- pt[0].fi) <= mid;
        for (int i = 0, j = 0; !f && i < n; ++i) {
            while (sqr(pt[i].fi - pt[j].fi) > mid) ++j;
            int hmin = min(bgmin[j], edmin[i+1]);
            int hmax = max(bgmax[j], edmax[i+1]);
            if (sqr(hmax-hmin) <= mid && max(sqr(hmin), sqr(hmax))+max(sqr(pt[i].fi), sqr(pt[j].fi)) <= mid) f = 1;
        }
        if (f) ed = mid;
        else bg = mid+1;
    }
    return bg;
}
int main() {
    //freopen("in.txt", "r", stdin);
    cin>>n;
    rp(i,n) {
        int x, y;
        scanf("%d%d", &x, &y);
        pt[i] = mp(x, y);
    }
    ll r = cal();
    rp(i,n) swap(pt[i].fi, pt[i].se);
    r = min(r, cal());
    cout<<r<<endl;
    return 0;
}
