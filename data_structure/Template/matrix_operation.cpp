#include<cstdio>
#include<iostream>
#include<string>
#include<cstring>
#include<algorithm>
#include<cmath>
#include<vector>
using namespace std;
typedef long long ll;
struct Matrix {
    typedef int T;
    const static int R = 70;
    const static int C = 70;
    const static T MOD = 1e9+7;
    T da[R][C];
    int r, c;
    Matrix(const T _da[][C], int _r, int _c):r(_r),c(_c) {memcpy(da, _da, sizeof da);}
    Matrix(const Matrix & p):r(p.r),c(p.c) {memcpy(da, p.da, sizeof da);}
    Matrix(const int _r, const int _c, int _ty = 0):r(_r),c(_c) {
        memset(da, 0, sizeof da);
        if (_ty)
            for (int i = 0; i< r; ++i)
                da[i][i] = T(1);
    }
    Matrix operator * (const Matrix & a) const {
        Matrix res(r, a.c);
        for (int i = 0; i< res.r; ++i)
            for (int j = 0; j< res.c; ++j)
                for (int k = 0; k< c; ++k) {
                    res.da[i][j] = ((long long)da[i][k] * a.da[k][j] + res.da[i][j])%MOD;
                }
        return res;
    }
    const T * operator [] (const int & i) const {
        return da[i];
    }
    T * operator [] (int i) {
        return da[i];
    }
};
Matrix pow(const Matrix & a, int n){
    Matrix res(a.r, a.c, 1), p = a;
    while (n)    {
        if (n & 1) res = res*p;
        p = p*p;
        n >>= 1;
    }
    return res;
}
const int FIBO[2][2] = {0,1,0,0};
const int MUL[2][2] = {0,1,1,1};
int main(){
#ifndef ONLINE_JUDGE
    freopen("in.txt", "r", stdin);
#endif
    Matrix bg(FIBO,2,2);
    bg = bg*pow(Matrix(MUL,2,2), 64);
    if (bg.da[0][1] != 7565)
        puts("fuck you");
    return 0;
}
