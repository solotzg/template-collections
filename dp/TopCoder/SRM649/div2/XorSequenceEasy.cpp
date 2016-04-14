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

const int N = 55;

struct XorSequenceEasy {
    int getmax(vector <int> A, int N);
};
int cal(int * m, int cnt) {
    int r = 0;
    rp(i, cnt) rep(j, i+1, cnt) if (m[i] < m[j]) ++r;
    return r;
}
int P[N];
int XorSequenceEasy::getmax(vector <int> A, int N) {
    int n = A.size();
    VI B(n);
    rp(i, n) P[i] = A[i] ^ N;
    int res = cal(P, n);
    int m = 0;
    while ((1<<m) != N) ++m;
    urp(pos, m-1, 0) {
        int k = 0xffffffff << pos;
        int rr = -1;
        for (int msk : {0, 1<<pos} ) {
            rp(i, n) P[i] = (A[i] & k)^msk;
            int sm = cal(P, n);
            if (sm > rr) {
                rr = sm;
                rp(i, n) B[i] = A[i] ^ msk;
            }
        }
        res = max(res, rr);
        A = B;
    }
    return res;
}

int main() {
    return 0;
}
