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
struct Tdetectived2 {
    int reveal(vector<string> s) {
        const static int N = (1<<18)+10;
        static int num[N], levels[20];
        const int inf = 0x3f3f3f3f;
        int n = s.size();
        vector<VI> S(n, VI(n));
        rp(i,n) rp(j,n) S[i][j] = s[i][j] - '0';
        int M = 1<<n, max_levels;
        mst(num, inf);
        num[1] = 0;
        rep(task, 1, M) if(num[task] != inf) {
            max_levels = -1;
            mst(levels, -1);
            rp(i,n) if((1<<i)&task) rp(j,n) levels[j]=max(levels[j], S[i][j]);
            rp(i,n) if(_0((1<<i)&task)) max_levels = max(max_levels, levels[i]);
            rp(i,n) if(_0((1<<i)&task) && levels[i]==max_levels)
                num[task|(1<<i)] = min(num[task|(1<<i)], num[task]+1);
        }
        int res = 0;
        rep(K,1,n) {
            int ans = inf;
            rp(task, M) if(task&(1<<K)) ans = min(ans, num[task]);
            res += ans*(K);
        }
        return res;
    }
};
int main() {
    auto a = new Tdetectived2;
    assert(a->reveal(
    {"0103122326302335", "1011563543411505", "0103661036520111", "3130226506025653", "1562015121602260", "2662105511354602", "2316550151110446", "3505151015062410", "2430215100055423", "6366111500421250", "3450631004015304", "0122051652101164", "2105240251510402", "3516264442314046", "3015604125060401", "5513026030442610"}
    ) == 640);
    return 0;
}

