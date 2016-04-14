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

struct SettingShield {
    long long getOptimalCost(int n, int h, int t, vector <int> val0, vector <int> a, vector <int> b, vector <int> m) {
        VI protection(n);
        protection[0] = val0[0];
        rep(i, 1, n) protection[i] = (1ll*a[0]*protection[i-1]%m[0]+b[0])%m[0];
        VI mx_right(n+2, -1);
        int left = val0[1], right = val0[2];
        mx_right[left] = max(mx_right[left], right);
        rep(i, 1, h) {
            int nleft = min((ll)n-1, (1ll*a[1]*left+b[1])%m[1] );
            int dist = right - left;
            int nright = min((ll)n-1, nleft + (1ll*a[2]*dist + b[2])%m[2]);
            mx_right[nleft] = max(mx_right[nleft], nright);
            left = nleft;
            right = nright;
        }
        rep(i, 1, n) if (mx_right[i-1] >= i) mx_right[i] = max(mx_right[i], mx_right[i-1]);
        int bg = 0, ed = *max_element(vtr(protection));
        rp(i, n) if (mx_right[i] == -1) {
            bg = max(bg, protection[i]);
        }
        auto cal = [&](int p) -> ll {
            ll cost = 1ll*p*t;
            static int pro[100005];
            mst(pro, 0);
            int pre = p;
            rp(i, n) {
                pre -= pro[i];
                if (protection[i] > pre) {
                    int f = protection[i]-pre;
                    cost += f;
                    pre += f;
                    pro[mx_right[i]+1 ] += f;
                }
            }
            return cost;
        };
        while (bg + 2 < ed) {
            int p = (ed-bg)/3;
            int c1 = bg+p;
            int c2 = ed-p;
            ll p1 = cal(c1), p2 = cal(c2);
            // printf("%d %d  %d %d %lld %lld\n", bg, ed , c1, c2, p1, p2);
            if (p1 == p2) bg = c1, ed = c2;
            else if (p1 > p2) bg = c1;
            else ed = c2;
        }
        ll res = 2e18;
        repd(x, bg, ed) {
            res = min(res, cal(x) );
        }
        return res;
    }
};
