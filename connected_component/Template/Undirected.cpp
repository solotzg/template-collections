const int N = 55;
struct edge{
    int v, nxt;
}e[N*4];
int ecnt, head[N];
void add(int u, int v){
    e[ecnt].v = v; e[ecnt].nxt = head[u]; head[u] = ecnt++;
}
int dfn[N], low[N], cnt;
int nblock;
int evis[N*4];
void init() {
    mst(head, -1);
    ecnt = 0;
    mst(dfn, 0);
    mst(low, 0);
    cnt = 0;
    nblock = 0;
    mst(evis, 0);
}
void tarjan(int u) {
    dfn[u] = low[u] = ++cnt;
    for (int i = head[u]; ~i; i=e[i].nxt) {
        int v = e[i].v;
        if (evis[i>>1]) continue;
        evis[i>>1] = 1;
        if (!dfn[v]) {
            tarjan(v);
            low[u] = min(low[v], low[u]);
            nblock += dfn[u]<low[v];
        } else if(dfn[v] < low[u]) {
            low[u] = dfn[v];
        }
    }
}
