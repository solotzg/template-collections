### https://community.topcoder.com/stat?c=round_overview&rd=16708
### http://apps.topcoder.com/wiki/display/tc/SRM+687

### AllGraphCuts
* 已知图中每个点对的最大流最小割，求出原图
* 任意点对(i,j)，则max_flow(i,j) >= min(max_flow(i,k), max_flow(k,j))
* 按照最大生成树构图

### Queueing
* 概率dp