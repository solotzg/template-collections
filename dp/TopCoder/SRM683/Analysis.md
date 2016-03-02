### https://community.topcoder.com/stat?c=round_overview&er=5&rd=16653

### MoveStones
* 比较trick的一题
* 至少存在一个位置，其移动方向是单向的，那么以该位置为基础可以将圆圈变成直线
* 原题地址：http://usaco.org/index.php?page=viewproblem2&cpid=128
* 题解：http://usaco.org/current/data/sol_restack.html

### GCDLCM2
* 题意：黑板上有N个数，每次可以去掉任意两个数a、b并添加gcd(a,b)、lcm(a,b)到黑板上，求黑板上的数总和最大是多少
* 设 num[N] = (2^2 * 3^3 * 5^4),(2^10 * 7^2),(2^4 * 3^6)
* 我们总能找出一种方法使得最后的 num[N] = (2^10 * 3^6 * 5^4 * 7^2),(2^4 * 3^3),(1)
* 即对于相同的素数底数从大到小排列