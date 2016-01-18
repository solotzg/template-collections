### https://community.topcoder.com/stat?c=round_overview&rd=16513
### http://apps.topcoder.com/wiki/display/tc/SRM+664

### BearPlays
* 比较trick的一题

#### 题意
* 给出两个数A和B，进行K次操作
* 每次操作：选出两数中较小的（设为A）， A <= 2*A、B <= (B-A)

#### 解法
* 原以为是找规律的题，但很不幸有些case会超时
* 任意取一个数A，若 B >= A ，则变成 2*A
* 否则变成 A-B，即 2*A-N => 2\*A%N

### BearAttacks
* 更加trick的一题
* 求一个连通分量中点数n的平方，等于找出连通的点对的数目
* (x+y+z)^2 => x^2 + y^2 + z^2 + 2xy + 2xz + 2yz
