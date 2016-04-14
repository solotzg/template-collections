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

struct TaroFillingAStringDiv1{
	
	int getNumber(int N, vector <int> p, string v){
		int n = p.size();
		rp(i, n) rep(j,i+1, n) if (p[i] > p[j]) {
			swap(p[i], p[j]);
			swap(v[i], v[j]);
		}
		int res = 1;
		const int MOD = 1e9+7;
		auto mul = [](int a, int b){ return (ll)a*b%MOD;};
		auto f = [&](int num, int eq){
			return eq?(num%2?1:1+num):(num%2?1+num:1);
		};
		rp(i, n-1) {
			res = mul(res, f(p[i+1]-p[i]-1, v[i]==v[i+1]));
		}
		res *= N/N;
		return res;
	}
};
