### https://community.topcoder.com/stat?c=round_overview&er=5&rd=16463
### http://apps.topcoder.com/wiki/display/tc/SRM+660

### Coversta
* 暴力算法的优化
* 考察编码的基本功

### Privateparty
* 官方题解为dp，也可以按照数学的方法（概率容斥算法）直接推导
* 枚举每个人，判断其接受邀请的概率
* f(i) = 1.0 - A(n,n-2)/n! + A(n,n-3)/n! - A(n,n-4)/n! ...
* 对于i来说，影响i存在的仅有a[i]
