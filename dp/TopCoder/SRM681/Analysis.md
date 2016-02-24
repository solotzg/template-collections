### https://community.topcoder.com/stat?c=round_overview&rd=16651
### http://apps.topcoder.com/wiki/display/tc/SRM+681

### FleetFunding
* 基本的二分+枚举

### LimitedMemorySeries2
* 暴力模拟，题中的推导公式可逆

### XorLists
* 状态位dp

### CoinFlips
* 由于每对点仅计算一次，所以dp[N][i][j]表示 当前N个点，下标为i和j的点对的概率
* 根据题意需要选择一个点并去掉，所以涉及到3种转换关系
* 去掉的点为j-N、i-j、1-i