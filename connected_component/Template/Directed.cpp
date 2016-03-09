const int N = 55;
struct edge{
    int v, nxt;
}e[N*4];
int ecnt, head[N];
void add(int u, int v){
    e[ecnt].v = v; e[ecnt].nxt = head[u]; head[u] = ecnt++;
}
int dfn[N], low[N], cnt, isin[N];
int vnblock[N], nblock, belong[N];
int mstk[N], top;
void init() {
    mst(head, -1);
    ecnt = 0;
    mst(dfn, 0);
    mst(low, 0);
    cnt = 0;
    mst(isin, 0);
    top = 0;
    nblock = 0;
    mst(vnblock, 0);
}
void tarjan(int u) {
    dfn[u] = low[u] = ++cnt;
    isin[u] = 1;
    mstk[++top] = u;
    for (int i = head[u]; ~i; i=e[i].nxt) {
        int v = e[i].v;
        if (!dfn[v]) {
            tarjan(v);
            low[u] = min(low[v], low[u]);
        } else if(isin[v]) {
            low[u] = min(low[u], dfn[v]);
        }
    }
    if (dfn[u] == low[u]) {
        int v = -1;
        do {
            v = mstk[top];
            isin[v] = 0;
            vnblock[nblock]++;
            --top;
            belong[v] = nblock;
        }while (u != v);
        ++nblock;
    }
}
