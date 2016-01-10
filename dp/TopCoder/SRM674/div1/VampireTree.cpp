#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
typedef long long ll;
struct VampireTree{
        int maxDistance(vector<int> num) {
        int n = num.size();
        int r = -1;
        rp(i,n) {
            vector<int> tp;
            int m = 1, s = num[i];
            rp(j,n)
                if(j!=i)
                    m += num[j]-1!=0, s += num[j]-1;
            if (s+1 != n)
                continue;
            r = max(r, m);
        }
        return r;
    }
};
int main() {
    VampireTree *v = new VampireTree;
    assert(v->maxDistance({3,1,1,3,1,2,1}) == 4);
    assert(v->maxDistance({2, 3, 3, 4, 1, 2, 3, 1, 1, 2, 2, 1, 1, 3, 1, 3, 1, 1, 1}) == 11);
    return 0;
}
