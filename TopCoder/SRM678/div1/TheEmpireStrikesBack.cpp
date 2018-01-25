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
struct TheEmpireStrikesBack{
    typedef pair<int,int> PII;
    const static int N = 100005;
    const int MOD = 1e9+7;
    int find(int AX, int BX, int CX, int AY, int BY, int CY, int n, int m) {
        static PII pos[N];
        pos[0].fi = AX;
        pos[0].se = AY;
        rep(i,1,n) {
            pos[i].fi = (((ll)pos[i-1].fi*BX)+CX)%MOD;
            pos[i].se = (((ll)pos[i-1].se*BY)+CY)%MOD;
        }
        sort(pos, pos+n);
        int l = 0, r = MOD, s;
        while (l< r) {
            int mid = (l+r)/2;
            s = n-1;
            int mxy = -1, cnt = 0;
            while (s >= 0) {
                int x = pos[s].fi, y = pos[s].se;
                while (s>=0 && pos[s].fi+mid>=x) y=max(y,pos[s].se), --s;
                mxy = max(mxy, y+mid);
                ++cnt;
                while (s>=0 && pos[s].se <= mxy) --s;
            }
            if (cnt <= m )
                r = mid;
            else
                l = mid+1;
        }
        return l;
    }
};
