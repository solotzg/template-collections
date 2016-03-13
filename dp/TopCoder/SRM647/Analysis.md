### https://community.topcoder.com/stat?c=round_overview&er=5&rd=16279
### http://apps.topcoder.com/wiki/display/tc/SRM+647

### BuildingTowers
* 貌似是比较经典的题，官方题解有 nlogn 的解法，但 n^2 的解法更容易理解
* 首先找出x中建筑的最大值，因为每个建筑的高度受相邻的影响，所以用扩增法
* 针对未知的建筑，可建立线性函数再求解，这里需要注意的是两直线交点的横坐标向上向下取整
