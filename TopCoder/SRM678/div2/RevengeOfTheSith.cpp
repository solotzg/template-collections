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
struct RevengeOfTheSith{
    typedef pair<int,int> PII;
    const static int N = 55;
    const int inf = 0x3f3f3f3f;
    int n, d, dp[N][N], vis[N][N];
    VI height;
    int f(int idx, int rem) {
        if (idx == n) {
            if (!rem)
                return 0;
            return inf;
        }
        if (vis[idx][rem]) return dp[idx][rem];
        dp[idx][rem] = inf;
        vis[idx][rem] = 1;
        repd(next_idx, idx+1, n) {
            int height_diff = height[next_idx] - height[idx];
            int cnt = next_idx - idx;
            int sum = 0;
            if (height_diff % cnt == 0) {
                int ele_height = height_diff / cnt;
                if (ele_height > d) sum += (cnt)*(ele_height-d)*(ele_height-d);
            } else {
                int ele_height = height_diff / (cnt);
                int height_left = height_diff - ele_height*cnt;
                if (ele_height > d) sum += (cnt - height_left)*(ele_height-d)*(ele_height-d);
                if (ele_height+1 > d) sum += height_left*(ele_height+1 - d)*(ele_height+1 - d);
            }
            sum += f(next_idx, rem-1);
            dp[idx][rem] = min(sum, dp[idx][rem]);
        }
        return dp[idx][rem];
    }
    int move(vector<int> steps, int T, int D) {
        n = steps.size();
        height.pb(0);
        for (int u : steps) height.pb(height.back() + u);
        d = D;
        mst(vis, 0);
        int ans = f(0, n-T);
        return ans;
    }
};
int main() {
    auto *t = new RevengeOfTheSith;
    assert(t->move({2,3,5},1,1) == 19);
    return 0;
}

