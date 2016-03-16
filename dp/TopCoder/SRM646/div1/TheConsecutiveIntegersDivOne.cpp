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

struct TheConsecutiveIntegersDivOne{
    int find(VI numbers, int k);
};

const int N = 50;
int TheConsecutiveIntegersDivOne::find(VI num, int k) {
    sort(vtr(num));
    int n = num.size(), res = 1e9;
    rp(i, n-k+1) {
        int bg = i, ed = i+k-1;
        for (int j = (bg+ed)/2; j <= (bg+ed+1)/2; ++j) {
            int x = num[j];
            for (int o = x-k+1; o <= x; ++o) {
                int sum = 0, y = o;
                for (int p = bg; p <= ed; ++p) {
                    sum += abs(y - num[p]);
                    ++y;
                }
                res = min(res, sum);
            }
        }
    }
    return res;
}

int main() {
    TheConsecutiveIntegersDivOne t;
    cout<<t.find(

{-96, -53, 82, -24, 6, -75},
4
                 )<<endl;
    return 0;
}
