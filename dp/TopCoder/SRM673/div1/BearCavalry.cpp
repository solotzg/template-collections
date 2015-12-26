#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
typedef long long ll;
struct BearCavalry {
    int countAssignments(vector <int> warriors, vector <int> horses) {
        const ll mod = 1e9+7;
        ll res = 0;
        vector<int> tp;
        int w = warriors[0];
        warriors.erase(warriors.begin());
        sort(warriors.begin(), warriors.end(), greater<int>());
        int lw = warriors.size(), lh = horses.size(), lt;
        rp(i, lh) {
            tp.clear();
            rp(j,lh)if(j!=i) tp.push_back(horses[j]);
            sort(tp.begin(), tp.end());
            lt = tp.size();
            int sm = w*horses[i];
            int ed = 0;
            ll u = 1;
            rp(j,lw) {
                while (ed<lt && warriors[j]*tp[ed]<sm) ++ed;
                if (ed >= lt) {
                    --ed;
                } else {
                    --ed;
                    if (ed < 0) {
                        u = 0;
                        break;
                    }
                }
                if (ed < j) {
                    u = 0;
                    break;
                }
                u = u*(ed-j+1)%mod;
            }
            res = (res+u)%mod;
        }
        return res;
    }
};
int main() {
    BearCavalry *b = new BearCavalry;
    cout<<b->countAssignments({7, 1, 2, 2}, {13, 19, 2, 1})<<endl;
    return 0;
}
