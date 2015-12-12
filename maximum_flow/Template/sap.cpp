#include <cstdio>
#include <cstring>
#include <cmath>
#include <iostream>
using namespace std;
struct SAP {
    const static int N = 210;
    const static int M = 210;
    const int INF = 0x3f3f3f3f;
    struct Edge {
        int v, w, nxt;
        Edge(int _v, int _w, int _nxt):v(_v),w(_w),nxt(_nxt) {}
        Edge() {}
    } ee[(N*2+M*2)*2];
    int head[N], cnt;
    int cur[N], dis[N], gap[N], pre[N];
    void add(int u, int v, int w) {
        ee[cnt] = Edge(v,w,head[u]);
        head[u] = cnt++;
        ee[cnt] = Edge(u,0,head[v]);
        head[v] = cnt++;
    }
    void init(){
        cnt = 0;
        memset(head, -1, sizeof head);
    }
    int sap(int start,int end,int nodenum) {
        memset(dis,0,sizeof dis);
        memset(gap,0,sizeof gap);
        memcpy(cur,head,sizeof cur);
        int u=pre[start]=start;
        int maxflow=0,aug=-1;
        gap[0]=nodenum;
        while(dis[start]<nodenum) {
loop:
            for(int &i=cur[u]; i!=-1; i=ee[i].nxt) {
                int v=ee[i].v;
                if(ee[i].w&&dis[u]==dis[v]+1) {
                    if(aug==-1||aug>ee[i].w)
                        aug=ee[i].w;
                    pre[v]=u;
                    u=v;
                    if(v==end) {
                        maxflow+=aug;
                        for(u=pre[u]; v!=start; v=u,u=pre[u]) {
                            ee[cur[u]].w-=aug;
                            ee[cur[u]^1].w+=aug;
                        }
                        aug=-1;
                    }
                    goto loop;
                }
            }
            int mindis=nodenum;
            for(int i=head[u]; i!=-1; i=ee[i].nxt) {
                int v=ee[i].v;
                if(ee[i].w && mindis>dis[v]) {
                    cur[u]=i;
                    mindis=dis[v];
                }
            }
            if((--gap[dis[u]])==0)break;
            gap[dis[u]=mindis+1]++;
            u=pre[u];
        }
        return maxflow;
    }
} _sap;
