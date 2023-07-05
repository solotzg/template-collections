#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
const int FIBO[2][2] = {0, 1, 0, 0};
const int MUL[2][2] = {0, 1, 1, 1};
const double F = log10(1 / sqrt(5.0));
const double Q = log10((1 + sqrt(5.0)) / 2.0);
struct Matrix {
  typedef int T;
  const static int R = 2;
  const static int C = 2;
  const static T MOD = 10000;
  T da[R][C];
  int r, c;
  Matrix(const T _da[][C], int _r, int _c) : r(_r), c(_c) {
    memcpy(da, _da, sizeof da);
  }
  Matrix(const Matrix &p) : r(p.r), c(p.c) { memcpy(da, p.da, sizeof da); }
  Matrix(int _r, int _c, int _ty = 0) : r(_r), c(_c) {
    memset(da, 0, sizeof da);
    if (_ty)
      for (int i = 0; i < r; ++i)
        da[i][i] = T(1);
  }
  Matrix operator*(const Matrix &a) const {
    Matrix res(r, a.c);
    for (int i = 0; i < res.r; ++i)
      for (int j = 0; j < res.c; ++j)
        for (int k = 0; k < c; ++k) {
          res.da[i][j] += da[i][k] * a.da[k][j];
          res.da[i][j] %= MOD;
        }
    return res;
  }
};
Matrix pow(Matrix a, int n) {
  Matrix res(a.r, a.c, 1), p = a;
  while (n) {
    if (n & 1)
      res = res * p;
    p = p * p;
    n >>= 1;
  }
  return res;
}
int mmp[50] = {0, 1, 1};
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif
  int n, dx;
  for (dx = 2; (mmp[dx] = mmp[dx - 1] + mmp[dx - 2]) < 100000000; ++dx)
    ;
  while (scanf("%d", &n) != EOF) {
    if (n < dx)
      printf("%d\n", mmp[n]);
    else {
      double as = F + Q * n;
      as = as - int(as);
      printf("%d...", int(1000.0 * pow(10.0, as)));
      Matrix bg(FIBO, 2, 2);
      bg = bg * pow(Matrix(MUL, 2, 2), n - 1);
      printf("%04d\n", bg.da[0][1]);
    }
  }
  return 0;
}
