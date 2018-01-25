#include <cstdio>
#include <iostream>
#include <string>
#include<assert.h>
#include <algorithm>
#include <vector>
#include <cstring>
#include <queue>
#include <set>
typedef long long int ll;
#define rp(i,b) for(int i=(0),__tzg_##i=(b);i<__tzg_##i;++i)
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define repd(i,a,b) for(int i=(a),__tzg_##i=(b);i<=__tzg_##i;++i)
#define mst(a,b) memset(a,b,sizeof(a))
using namespace std;
struct SubdividedSlimes {
    int needCut(int S, int M) {
        rep(i,2,S+1) {
            int a = S/(i), b = S%(i), s = S;
            int r = 0;
            rep(j, 0, i) {
                int d = a;
                if (j+b >= i)
                    ++d;
                r += d*(s-d);
                s -= d;
            }
            if (r >= M)
                return i-1;
        }
        return -1;
    }
};
