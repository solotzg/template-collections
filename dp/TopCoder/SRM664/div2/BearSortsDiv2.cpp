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
struct BearSortsDiv2{
    const static int N = 45;
    VI T;
    int merged[N], cnt = 0;
    int cmp[N][N];
    void mergeSort (int left, int right) {
        if (left + 1 >= right) return ;
        int mid = (left+right)/2;
        mergeSort(left, mid);
        mergeSort(mid,right);
        int p1 = left, p2 = mid, n = 0;
        while (p1 < mid || p2 < right) {
            if (p1 == mid) merged[n++] = T[p2++];
            else if(p2 == right) merged[n++] = T[p1++];
            else {
                ++cnt;
                if (cmp[T[p1]][T[p2]]) merged[n++] = T[p1++];
                else merged[n++] = T[p2++];
            }
        }
        rep(i,left,right) T[i] = merged[i-left];
    }
    double getProbability(vector <int> seq) {
        mst(cmp, 0);
        rp(i,seq.size()) rep(j,i+1,seq.size()) cmp[seq[i]][seq[j]] = 1;
        rp(i,seq.size()) T.pb(i+1);
        mergeSort(0, T.size());
        return log(pow(0.5, cnt));
    }
};
int main() {
    auto a = new BearSortsDiv2;
    cout<<(a->getProbability({10,13,18,2,4,6,24,22,19,5,7,20,23,14,21,17,25,3,1,11,12,8,15,16,9}))<<endl;
    return 0;
}

