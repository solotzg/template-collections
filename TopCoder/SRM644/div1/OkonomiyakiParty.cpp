#include<bits/stdc++.h>
using namespace std;
typedef long long ll;
struct OkonomiyakiParty {
    const static int mod = 1e9+7, N = 55;
    inline int add(int a, int b) {
        a += b;
        return a >= mod ? a-mod : a;
    }
    int comb[N][N];
    void init() {
        memset(comb, 0, sizeof comb);
        for (int i = 0; i< N; ++i) {
            comb[i][0] =comb[i][i] = 1;
            for (int j = 1; j< i; ++j) {
                comb[i][j] = add(comb[i-1][j-1], comb[i-1][j]);
            }
        }
    }
    int count(vector <int> o, int M, int K) {
        init();
        sort(o.begin(), o.end());
        int n = o.size();
        int res = 0;
        for (int i = 0; i< n; ++i) {
            int x = o[i];
            int dx = upper_bound(o.begin()+i, o.end(), x+K) - o.begin();
            int p = dx - i;
            res = add(res, comb[p-1][M-1] );
        }
        return res;
    }
};
