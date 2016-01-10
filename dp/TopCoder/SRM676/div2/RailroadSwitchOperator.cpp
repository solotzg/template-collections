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
typedef pair<int, int> PII;
typedef vector<PII> VPII;
typedef long long ll;
struct RailroadSwitchOperator {
    int minEnergy(int N, vector <int> x, vector <int> t) {
        vector<tuple<int, int, int>> events;
        rp(i, x.size()) {
            int p = 0, xx = x[i] - 1, st = t[i];
            int l = 0, r = N - 1;
            while(1) {
                if(l == r) break;
                int m = (l + r) / 2;
                if(xx <= m) {
                    r = m;
                    events.pb(make_tuple(st, p, -1));
                    p = p * 2 + 1;
                }
                else {
                    l = m + 1;
                    events.pb(make_tuple(st, p, 1));
                    p = 2 * p + 2;
                }
                ++st;
            }
        }
        sort(vtr(events));
        vector<int> dir(2 * N, -1), last(2 * N, 0);
        int ac = 0, res = 0;
        for(auto e : events) {
            int t, p, d;
            tie(t, p, d) = e;
            if(last[p] < ac) {
                last[p] = t;
                dir[p] = d;
            }
            else {
                if(dir[p] != d) {
                    ++res;
                    dir[p] = d;
                    ac = t;
                }
                last[p] = t;
            }
        }
        return res;
    }
};
int main() {
    return 0;
}
