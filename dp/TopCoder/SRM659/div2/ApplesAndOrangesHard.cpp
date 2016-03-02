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

struct ApplesAndOrangesHard {
    int maximumApples(int N, int K, vector <int> info) {
        for (int & a : info) --a;
        sort(vtr(info));
        VI d(K,0);
        rp(i,K/2) d[i]=1;
        int res = (N/K)*(K/2);
        for (const int f : info ) {
            int a = f/K, b = f%K;
            a += 2;
            if (!d[b]) {
                rep(i,1,K) {
                    int o = (b+K-i)%K;
                    if (!d[o]) continue;
                    if (o<b && d[o]==a) continue;
                    if (o>b && d[o]>=a-1) continue;
                    d[o] = 0;
                    if (o>b) --res;
                    break;
                }
            }
            d[b] = a;
        }
        rp(i,N%K) if (d[i]) ++res;
        return res;
    }
};

int main() {
    ApplesAndOrangesHard a;
    cout<<a.maximumApples(
            10,3,{3,8}
        )<<endl;
    return 0;
}
