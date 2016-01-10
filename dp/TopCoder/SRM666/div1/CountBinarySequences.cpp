#include <cstdio>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstring>
#include <queue>
#include <set>
#include <cmath>
#include <map>
#include <queue>
#include <stack>
using namespace std;
typedef long long ll;
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b); i>=__tzg_##i; --i)
#define rp(i,b) for(int i=(0), __tzg_##i=(b);i<__tzg_##i;++i)
#define rep(i,a,b) for(int i=(a), __tzg_##i=(b);i<__tzg_##i;++i)
#define repd(i,a,b) for(int i=(a), __tzg_##i=(b);i<=__tzg_##i;++i)
#define mst(a,b) memset(a,b,sizeof(a))
typedef pair<int,int> pii;
#define px first
#define py second
const ll mod = 1000000007;
const int MAXN = 4005;
const int MAXM = 15;
const double eps = 1e-6;
#define mp(a,b) make_pair(a,b)
typedef vector<int> VI;
typedef vector<pii> VPII;
#define px first
#define py second

struct Matrix {
    ll d[MAXM][MAXM];
    int r, c;
    void one() {
        rp(i,r) d[i][i] = 1;
    }
    Matrix(int rr, int cc):r(rr),c(cc) {
        mst(d,0);
    }
    Matrix() {
        mst(d,0);
        r = c = -1;
    }
    Matrix operator * (const Matrix & a) const {
        Matrix res(r,a.c);
        ll (*dd)[MAXM] = res.d;
        rp(i,res.r) {
            rp(j,res.c) {
                rp(k,c) {
                    dd[i][j] = (dd[i][j]+d[i][k]*a.d[k][j]%mod)%mod;
                }
            }
        }
        return res;
    }
    Matrix operator + (const Matrix & a) const {
        Matrix res(r,c);
        rp(i,r)rp(j,c) {
            res.d[i][j] = (d[i][j] + a.d[i][j])%mod;
        }
        return res;
    }
     void show() {
        rp(i,r) {
            rp(j,c) {
                printf("%lld ", d[i][j]);
            }
            puts("");
        }
        puts("");
    }
};
Matrix pow(const Matrix &a, ll n) {
    Matrix res(a.r, a.c);
    res.one();
    Matrix p = a;
    while (n > 0) {
        if (n & 1) res = res*p;
        p = p*p;
        n >>= 1;
    }
    return res;
}
Matrix base_matrix(-1,-1);
struct CountBinarySequences {
    int n, k;
    vector<int> L, R;
    int countSequences(int n, int k, vector <int> L, vector <int> R);
    void getInit() {
        base_matrix = Matrix(2*(k+1), 2*(k+1));
        rp(i, k+1) {
            base_matrix.d[i][0] = 1;
            base_matrix.d[i+1+k][1+k] = 1;
            if (i) {
                base_matrix.d[i+k][i] = 1;
                base_matrix.d[i-1][i+1+k] = 1;
            }
        }
    }
    Matrix solve(int i) {
        int a, b;
        if (i == -1) {
            a = 1,b = n;
        } else {
            a = L[i], b = R[i];
        }
        vector<Matrix> mat[2];
        Matrix zero(k+1,k+1);
        int t = a;
        while (t <= b) {
            pair<int,pair<int,int>> left_most = {b+1, {0,-1}};
            rp(j,L.size()) {
                if ((j!=i) && (L[j]>=t) && ((i==-1)||(R[j]<=R[i]))) {
                    left_most = std::min(left_most, {L[j], {-R[j],j}});
                }
            }
            if (left_most.second.second == -1) {
                int m = b+1-t;
                mat[0].push_back(even_matrix(m));
                mat[1].push_back(odd_matrix(m));
                t = b+1;
            } else {
                int j = left_most.second.second;
                int m = left_most.first - t;
                mat[0].push_back(even_matrix(m));
                mat[1].push_back(odd_matrix(m));
                Matrix Y = solve(j);
                mat[0].push_back(Y);
                mat[1].push_back(zero);
                t = R[j]+1;
            }
        }
        Matrix r[2];
        r[0] = Matrix(k+1,k+1);
        r[0].one();
        r[1] = zero;
        rp(j,mat[0].size()) {
            Matrix A,B,C,D;
            A = r[0]*mat[0][j];
            B = r[1]*mat[1][j];
            C = r[0]*mat[1][j];
            D = r[1]*mat[0][j];
            r[0] = A+B;
            r[1] = C+D;
        }
        if (i == -1)
            return r[0]+r[1];
        else
            return r[0];
    }
    Matrix even_matrix(int m) {
        Matrix X = pow(base_matrix, m);
        Matrix R(k+1, k+1);
        rp(i,k+1) rp(j,k+1) {
            R.d[i][j] = X.d[i][j];
        }
        return R;
    }
    Matrix odd_matrix(int m) {
        Matrix X = pow(base_matrix, m);
        Matrix R(k+1, k+1);
        rp(i,k+1) rp(j,k+1) {
            R.d[i][j] = X.d[i][j+k+1];
        }
        return R;
    }
};
int CountBinarySequences::countSequences(int _n, int _k, vector <int> _L, vector <int> _R) {
    n = _n;
    k = _k;
    L = _L;
    R = _R;
    getInit();
    Matrix X = solve(-1);
    ll s = 0;
    rp(i,k+1)
    s = (s + X.d[0][i])%mod;
    return s;
}
