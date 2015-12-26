#include<cstdio>
#include<iostream>
#include<string>
#include<cstring>
#include<algorithm>
#include<cmath>
#include<vector>
#include<set>
#include<queue>
using namespace std;
#define rep(i,a,b) for(int i(a), __tzg##i(b);i<__tzg##i;++i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,1+b)
#define urp(i,a,b) for(int i(a), __tzg##i(b);i>=__tzg##i;--i)
typedef long long ll;
const static int N = 21;
ll fac[N], stirl[N][N];
void init() {
    fac[0] = 1;
    rep(i,1,N) fac[i] = fac[i-1]*i;
    stirl[0][0] = 1;
    rep(i,1,N) {
        stirl[i][0] = 0;
        repd(j,1,i)
            stirl[i][j] = stirl[i-1][j-1] + (i-1)*stirl[i-1][j];
    }
}
int main(){
#ifndef ONLINE_JUDGE
    freopen("in.txt", "r", stdin);
#endif
    init();
    int t;
    cin>>t;
    while (t--) {
        int n, k;
        cin>>n>>k;
        ll s1 = 0, s2 = fac[n];
        repd(i,1,k)
            s1 += stirl[n][i] - stirl[n-1][i-1];
        printf("%.04lf\n", (double)s1/s2);
    }
    return 0;
}
