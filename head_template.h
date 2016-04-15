const static int MOD = 1e9+7;
inline int mul (int a, int b) {
    return (ll)a*b%MOD;
}
inline int add (int a, int b) {
    (a+=b)>=MOD?a-MOD:(a<0?a+MOD:a);
}
int pow(int a, int b) {
    int r = 1, p = a;
    while (b) {
        if (b & 1) r = mul(r, p);
        p = mul(p, p);
        b >>= 1;
    }
    return r;
}

{
    int comb[N][N];
    rp(i,N) {
        comb[i][0] = comb[i][i] = 1;
        rep(j, 1, i) {
            comb[i][j] = add(comb[i-1][j-1], comb[i-1][j]);
        }
    }
}

{
    int A[N], RA[N];
    A[0] = RA[0] = 1;
    rep(i,1,N) A[i] = mul(A[i-1], i), RA[i] = pow(A[i], MOD-2);
}

int get_comb(int n, int k) {
    return mul(A[n], mul(RA[k], RA[n-k]));
}

template<class T, class U>
T cast (U x) {
    T y;
    ostringstream a;
    a<<x;
    istringstream b(a.str());
    b>>y;
    return y;
}
template<class T>
vector<T> split(string s, string x=" ") {
    vector<T> r;
    rp(i,s.size()) {
        string c;
        while(i<(int)s.size()&&x.find(s[i])==string::npos) c+=s[i++];
        if(c.size()) r.push_back(cast<T>(c));
    }
    return r;
}
