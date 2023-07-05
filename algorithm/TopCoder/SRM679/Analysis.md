### https://community.topcoder.com/stat?c=round_overview&rd=16649
### http://apps.topcoder.com/wiki/display/tc/SRM+679

### RedAndBluePoints
* 计算几何+dp
* 先按 y,x 排序， 枚举最左下角的点o，将剩余的点按极角排序
* 其中求dp是关键，用dp(i，j)表示凸包最后一条边为i->j时凸包内的点数，那么枚举k，更新dp(j,k)

### ForbiddenStreets
* 虽说按照题目限度暴力的解法也是可以的，但是时间复杂度为 O(n^3 * m)
* 另一种解法为找出任意两点u,v之间的最短路径数目ways(u,v)
* 对于点st,ed，如果ways(st,u)*ways(v,ed) == ways(st,ed)，表明删掉u,v则st和ed间的最短路径必然会被改变
