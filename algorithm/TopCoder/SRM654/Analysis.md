### https://community.topcoder.com/stat?c=round_overview&rd=16318
### http://apps.topcoder.com/wiki/display/tc/SRM+654

### SuccessiveSubtraction2
* 关键在于将括号拆分后得出符号的规律

### TwoEntrances
* 如果只考虑一个入口，那么dp关系显而易见
* 当考虑两个入口s1 s2时，两个入口之一必然是最后一个需要考虑的
* 分别假设s1或s2是最后考虑，那么与其相连的子树中的点集可以随意摆放在长度为n-1的序列中的任何位置
* 连接s1和s2路径上且与其相邻的点又变成了新的两个入口的问题