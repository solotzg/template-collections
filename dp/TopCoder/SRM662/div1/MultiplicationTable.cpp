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
struct MultiplicationTable {
    const static int N = 55;
    const int inf = 0x3f3f3f3f;
    int v[N][N], idx_step[N];
    vector <int> getMultiplicationTable(vector <int> firstRow) {
        int n = firstRow.size();
        VI no({-1});
        rp(i,n) {
            v[i][0] = i;
            rp(j,n) v[i][j+1] = firstRow[v[i][j]];
        }
        mst(idx_step, -1);
        int repStep = -1, curStep = -1;
        for (int j=0; ;j++) {
            int k = v[0][j];
            if (idx_step[k] != -1) {
                repStep = idx_step[k];
                curStep = j+1;
                break;
            }
            idx_step[k] = j+1;
        }
        rp(k,n) if (v[k][repStep]!=firstRow[v[k][curStep-1]])
            return no;
        rp(i,n) {
            if (idx_step[i] != -1) continue;
            repd(j,0,n) {
                int k = v[i][j];
                if (idx_step[k] != -1) {
                    idx_step[i] = idx_step[k]-j;
                    break;
                }
            }
        }
        VI res;
        rp(i,n)rp(j,n) res.pb(idx_step[i]==-1?i:v[j][idx_step[i]]);
        return res;
    }
};
int main(){
    MultiplicationTable m;
    vector<int> v = m.getMultiplicationTable({0,1});
    return 0;
}
