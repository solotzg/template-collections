#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vtr(v) (v).begin(),(v).end()
#define mp(a,b) make_pair(a,b)
#define fi first
#define se second
#define pb(a) push_back(a)
#define _0(x) (!(x))
#define _1(x) (x)
#define bit(x,y) (((x)>>(y))&1)
#define siz(x) ((int)(x).size())
template<typename T>inline void smin(T & x, const T &y) {x=min(x,y);}
template<typename T>inline void smax(T & x, const T &y) {x=max(x,y);}
typedef long long ll;
typedef vector<int> VI;
typedef vector<ll> VLL;
typedef pair<int,int> PII;
typedef vector<PII> VPII;
