#include<cstdio>
#include<algorithm>
#include<vector>
#include<cmath>
#include<queue>
using namespace std;
struct IntegerPartition {
    const static int N = 100005;
    typedef long long ll;
    ll MOD = 1000000007;
    ll num[N];
    void init(int n = 100000) {
        num[0] = num[1] = 1;
        for (int i = 2; i<= n; ++i) {
            ll flg = 1;
            for (int k = 1; ; ++k) {
                int a = i - (3*k-1)*k/2, b = i - (3*k+1)*k/2;
                ll s1, s2;
                if (a < 0) break;
                s1 = num[a];
                if (b < 0) s2 = 0;
                else s2 = num[b];
                num[i] = (num[i]+flg*(s1+s2))%MOD;
                flg = 0-flg;
            }
            num[i] = (num[i]+MOD)%MOD;
        }
    }
};
struct IntegerPartitionRepeatLimit {
    const static int N = 100005;
    typedef long long ll;
    ll MOD = 1000000007;
    ll num[N];
    void init(int n) {
        num[0] = num[1] = 1;
        for (int i = 2; i<= n; ++i) {
            ll flg = 1;
            for (int k = 1; ; ++k) {
                int a = i - (3*k-1)*k/2, b = i - (3*k+1)*k/2;
                ll s1, s2;
                if (a < 0) break;
                s1 = num[a];
                if (b < 0) s2 = 0;
                else s2 = num[b];
                num[i] = (num[i]+flg*(s1+s2))%MOD;
                flg = 0-flg;
            }
            num[i] = (num[i]+MOD)%MOD;
        }
    }
    ll get_num(int a, int k) {
        ll res = num[a];
        for (int j = 1, f = -1; ; ++j, f=0-f)
        {
            int c = (3*j-1)*j/2*k, d = (3*j+1)*j/2*k;
            ll s1, s2;
            if (c > a) break;
            s1 = f*num[a-c];
            if (d > a) s2 = 0;
            else s2 = f*num[a-d];
            res = (res + s1 + s2)%MOD;
        }
        res = (res+MOD)%MOD;
        return res;
    }
};
int main() {
    IntegerPartition * test1 = new IntegerPartition();
    test1->init(200);
    printf("%lld\n", test1->num[5]);
    printf("%lld\n", test1->num[11]);
    printf("%lld\n", test1->num[15]);
    printf("%lld\n", test1->num[19]);
    /**
        7
        56
        176
        490
    */
    IntegerPartitionRepeatLimit *test2 = new IntegerPartitionRepeatLimit();
    test2->init(10);
    printf("%lld\n", test2->get_num(4,2));
    printf("%lld\n", test2->get_num(4,3));
    printf("%lld\n", test2->get_num(4,4));
    printf("%lld\n", test2->get_num(4,5));
    /**
        2
        4
        4
        5
    */
    return 0;
}
