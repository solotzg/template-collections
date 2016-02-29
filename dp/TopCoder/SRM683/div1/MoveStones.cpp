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

struct MoveStones{
     long long get(vector <int> a, vector <int> b){
         int n = a.size();
         vector<ll> s(n);
         rp(i, n) s[i] = (ll)a[i]-b[i];
         if (accumulate(s.begin(), s.end(), 0ll)) return -1;
         ll res = 1e18;
         rp(i, n) {
             ll c = 0, o = 0;
             rp(j, n) {
                 c += s[(i+j)%n];
                 o += abs(c);
             }
             res = min(res, o);
         }
         return res;
     }
};
