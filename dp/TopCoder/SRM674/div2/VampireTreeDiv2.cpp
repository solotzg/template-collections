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
struct VampireTreeDiv2 {
    const int mod = 1e9+7;
    const static int N = 1000;
    const int inf = 0x3f3f3f3f;
    int forced[N], num[N][2], cnt[N][2];
    VI child[N];
    VI parents;
    int countMinSamples(vector<int> A, vector<int> B) {
        int n = A.size() + 1;
        rp(i,A.size()) {
            if (B[i] == -1) {
                child[A[i]].pb(i+1);
            } else {
                parents.pb(i+1);
            }
        }
        int rnum = inf, rcnt = 0;
        int o = parents.size();
        rp(mask, 1<<o) {
            mst(forced, 0);
            int f = 1;
            rp(j,o) {
                int x = parents[j];
                int p1 = A[x-1], p2 = B[x-1];
                if ((1<<j) & mask) {
                    if (forced[x] == -1)
                        f = 0;
                    else
                        forced[x] = 1;
                } else {
                    if (forced[p1] == -1 || forced[p2] == -1)
                        f = 0;
                    else
                        forced[p1] = forced[p2] = 1;
                    if (forced[x] == 1)
                        f = 0;
                    else
                        forced[x] = -1;
                }
                if(!f) break;
            }
            if (!f) continue;
            urp(x, n-1, 0) {
                rp(must,2) {
                    int tmp_num;
                    int tmp_cnt;
                    num[x][must] = inf;
                    cnt[x][must] = 0;
                        // not must
                    if (must == 0 && (forced[x] != 1)) {
                        tmp_num = 0;
                        tmp_cnt = 1;
                        for (int v : child[x]) {
                            tmp_num += num[v][1];
                            tmp_cnt = (ll)tmp_cnt * cnt[v][1] % mod;
                        }
                        num[x][must] = tmp_num;
                        cnt[x][must] = tmp_cnt;
                    }
                        // take the node
                    if (forced[x] != -1) {
                        tmp_num = 1;
                        tmp_cnt = 1;
                        for (int v : child[x]) {
                            tmp_num += num[v][0];
                            tmp_cnt = (ll)tmp_cnt * cnt[v][0] % mod;
                        }
                        if (tmp_num < num[x][must]) {
                            num[x][must] = tmp_num;
                            cnt[x][must] = tmp_cnt;
                        } else if (tmp_num == num[x][must]) {
                            cnt[x][must] = (cnt[x][must] + tmp_cnt) % mod;
                        }
                    }
                }
            }
            int cnum = num[0][0];
            int ccnt = cnt[0][0];
            rp(i,o) {
                int x = parents[i];
                cnum += num[x][0];
                ccnt = (ll)ccnt * cnt[x][0] % mod;
            }
            if (cnum < rnum) {
                rnum = cnum;
                rcnt = ccnt;
            } else if (cnum == rnum) {
                rcnt = (rcnt + ccnt) % mod;
            }
        }
        return rcnt;
    }
};
int main() {
    VampireTreeDiv2 v;
    assert(v.countMinSamples(
                             {0,1,0,3,0,5,0,7,0,9,0,11,0,13,0,15,0,17,0,19,0,21,0,23,0,25,0,27,0,29,0,31,0,33,0,35,0,37,0,39,0,41,0,43,0,45,0,47,0,49,0,51,0,53,0,55,0,57,0,59,0},
                             {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
                             ) == 73741818);
    return 0;
}

