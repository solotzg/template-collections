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

const int N = 132000;

struct XorSequence {
    ll getmax(int N, int sz, int A0, int A1, int P, int Q, int R);
};
int tp[N];
ll merg(int * m , int bg, int ed) {
    if (bg == ed) return 0;
    ll res = 0;
    int mid = (bg+ed) /2;
    ll nl = merg(m, bg, mid);
    ll nr = merg(m, mid+1, ed);
    int o = lower_bound(m+bg, m+mid+1, m[mid+1]) - (m+bg);
    res += o;
    repd(i, mid+2, ed) {
        while (o+bg <= mid && m[o+bg] < m[i]) ++o;
        res += o;
    }
    merge(m+bg, m+mid+1, m+mid+1, m+ed+1, tp+bg);
    memcpy(m+bg, tp+bg, 4*(ed-bg+1));
    return res+nl+nr;
}
ll cal(int * m, int cnt) {
    return merg(m, 0, cnt-1);
}
int P[N];
ll XorSequence::getmax(int N, int sz, int A0, int A1, int L, int Q, int R){
    int n = sz;
    VI A(n);
    A[0] = A0;
    A[1] = A1;
    rep(i, 2, n) {
        ll t = (ll)A[i-2]*L%N;
        t += (ll)A[i-1]*Q%N;
        t %= N;
        t += R;
        t %= N;
        A[i] = t;
    }
    VI B(n);
    rp(i, n) P[i] = A[i] ^ N;
    ll res = cal(P, n);
    int m = 0;
    while ((1<<m) != N) ++m;
    urp(pos, m-1, 0) {
        int k = 0xffffffff << pos;
        ll rr = -1;
        for (int msk : {0, 1<<pos} ) {
            rp(i, n) P[i] = (A[i] & k)^msk;
            ll sm = cal(P, n);
            if (sm > rr) {
                rr = sm;
                rp(i, n) B[i] = A[i] ^ msk;
            }
        }
        res = max(res, rr);
        A = B;
    }
    return res;
}

int main() {
    XorSequence x;
    cout<<x.getmax(
                   4,6,3,2,0,1,3
                   )<<endl;
    return 0;
}
