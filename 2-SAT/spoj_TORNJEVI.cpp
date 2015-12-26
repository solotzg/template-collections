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
const double eps = 1e-6;
#define mp(a,b) make_pair(a,b)
typedef vector<int> VI;
#define px first
#define py second
const int N = 105;
char mmp[N][N];
int ndx[N][N], sT, sn, r, s, nn;
const int dir[4][2] = {{-1,0}, {0,-1}, {0,1}, {1,0}};
int rdir[4][4];
vector<pii> vT, vn;
vector<VI> vr, rv, bv, blv;
VI rl, color, vis1, vis2, vtk, bl, indu;
int top, cbl;
struct Node {
    int Tdx[4];
    Node() {
        mst(Tdx, -1);
    }
};
vector<Node> Tvn;
inline int getPos(int _dx, int _dir) {
    return (_dx<<2)+_dir;
}
inline int getRev(int _dx, int _dir) {
    return (_dx<<2)+3-_dir;
}
inline int getRev(int x) {
    return (x&0xffffffc) | (3-(x&3));
}
inline void add(int u, int v) {
    vr[u].push_back(v);
    rv[v].push_back(u);
}
void cal(int _dir, int i, int role = 0) {
    pii p;
    if (!role) p = vn[i];
    else p = vT[i];
    for (int _x=p.px+dir[_dir][0], _y=p.py+dir[_dir][1], k=1;
            k&&_x>=0&&_x<r&&_y>=0&&_y<s;
            _x+=dir[_dir][0], _y+=dir[_dir][1]) {
        switch(mmp[_x][_y]) {
        case 'T':
            if (!role)
                Tvn[i].Tdx[_dir] = ndx[_x][_y];
            else
                add(getPos(i, _dir), getRev(i, _dir));
            k = 0;
            break;
        case '#':
            k = 0;
            break;
        case 'n':
            if (role)
                break;
        }
    }
}
void topo() {
    queue<int> q1, q2;
    rp(i, indu.size()) if (indu[i] == 0)
        q1.push(i);
    color.assign(cbl, 0);
    while (!q1.empty()) {
        int cur = q1.front();
        q1.pop();
        if (color[cur]) continue;
        color[cur] = 1;
        rp(i, blv[cur].size()) {
            int pos = getRev(blv[cur][i]);
            q2.push(bl[pos]);
            while (!q2.empty()) {
                pos = q2.front();
                q2.pop();
                if (color[pos]) continue;
                color[pos] = 2;
                rp(j, bv[pos].size())
                if (!color[bv[pos][j]])
                    q2.push(bv[pos][j]);
            }
        }
        rp(i, bv[cur].size()) {
            --indu[bv[cur][i]];
            if (indu[bv[cur][i]] == 0)
                q1.push(bv[cur][i]);
        }
    }
}
void dfs1(int x) {
    vis1[x] = 1;
    rp(i, vr[x].size()) {
        if (!vis1[vr[x][i]])
            dfs1(vr[x][i]);
    }
    vtk[top++] = x;
}
void dfs2(int x) {
    vis2[x] = 1;
    bl[x] = cbl;
    rp(i, rv[x].size()) {
        if (!vis2[rv[x][i]])
            dfs2(rv[x][i]);
    }
}
void init() {
    vis1.assign(rv.size(), 0);
    vis2.assign(rv.size(), 0);
    vtk.assign(rv.size(), 0);
    bl.assign(rv.size(), 0);
    top = 0;
    cbl = 0;
}
void check() {
    rp(i, vr.size()) if (!vis1[i])
        dfs1(i);
    urp(i, top-1, 0) if (!vis2[vtk[i]])
        dfs2(vtk[i]), cbl++;
    blv.assign(cbl, VI());
    rp(i, rv.size())
    blv[bl[i]].push_back(i);
    bv.assign(cbl, VI());
    indu.assign(cbl, 0);
    rp(i, rv.size()) rp(j, rv[i].size()) if (bl[i] != bl[rv[i][j]])
        bv[bl[i]].push_back(bl[rv[i][j]]),
        ++indu[bl[rv[i][j]]];
}

int main() {
#ifndef ONLINE_JUDGE
    // freopen("in.txt", "r", stdin);
#endif
    scanf("%d%d", &r, &s);
    rp(i, r) scanf("%s", mmp[i]);
    rep(i, 0, r)rep(j, 0, s) {
        switch(mmp[i][j]) {
        case 'n':
            vn.push_back(mp(i,j));
            ndx[i][j] = sn++;
            break;
        case 'T':
            vT.push_back(mp(i,j));
            ndx[i][j] = sT++;
            break;
        }
    }
    nn = sT<<1;
    vr.assign(nn<<1, VI());
    rv = vr;
    Tvn.assign(sn, Node());
    rp(i, sT) rp(j, 4) cal(j, i, 1);
    rep(i, 0, sn) {
        rep(j, 0, 4) cal(j, i);
        int da = -1, db = -1, fa, fb;
        if (Tvn[i].Tdx[0] != -1 )
            da = Tvn[i].Tdx[0], fa = 3;
        if (Tvn[i].Tdx[3] != -1 ) {
            if (da != -1)
                da = -1,
                add(getPos(Tvn[i].Tdx[0], 3), getRev(Tvn[i].Tdx[0], 3)),
                add(getPos(Tvn[i].Tdx[3], 0), getRev(Tvn[i].Tdx[3], 0));
            else
                da = Tvn[i].Tdx[3], fa = 0;
        }
        if (Tvn[i].Tdx[1] != -1 )
            db = Tvn[i].Tdx[1], fb = 2;
        if (Tvn[i].Tdx[2] != -1 ) {
            if (db != -1)
                db = -1,
                add(getPos(Tvn[i].Tdx[1], 2), getRev(Tvn[i].Tdx[1], 2)),
                add(getPos(Tvn[i].Tdx[2], 1), getRev(Tvn[i].Tdx[2], 1));
            else
                db = Tvn[i].Tdx[2], fb = 1;
        }
        if (da != -1 && db != -1)
            add(getRev(da, fa), getPos(db, fb)),
                add(getRev(db, fb), getPos(da, fa));
        else if (da != -1)
            add(getRev(da, fa), getPos(da, fa));
        else if (db != -1)
            add(getRev(db, fb), getPos(db, fb));
    }
    init();
    check();
    topo();
    rdir[1][3] = 1;
    rdir[1][0] = 4;
    rdir[2][3] = 2;
    rdir[2][0] = 3;
    rp(i, sT) {
        pii ap = vT[i];
        int dx = ndx[ap.first][ap.second];
        int _x, _y;
        int a = getPos(dx, 0);
        if (color[bl[a]] == 1)
            _x = 0;
        else
            _x = 3;
        a = getPos(dx, 1);
        if (color[bl[a]] == 1)
            _y = 1;
        else
            _y = 2;
        mmp[ap.first][ap.second] = '0' + rdir[_y][_x];
    }
    rp(i, r) puts(mmp[i]);
    return 0;
}
