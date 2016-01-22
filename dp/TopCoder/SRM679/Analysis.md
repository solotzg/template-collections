### https://community.topcoder.com/stat?c=round_overview&rd=16649
### http://apps.topcoder.com/wiki/display/tc/SRM+679

### RedAndBluePoints
* 计算几何+dp
* 先按 y,x 排序， 枚举最左下角的点o，将剩余的点按极角排序
* 其中求dp是关键，用dp(i，j)表示凸包最后一条边为i->j时凸包内的点数，那么枚举k，更新dp(j,k)
