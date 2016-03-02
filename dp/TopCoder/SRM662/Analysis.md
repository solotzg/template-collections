### https://community.topcoder.com/stat?c=round_overview&er=5&rd=16511
### http://apps.topcoder.com/wiki/display/tc/SRM+662

### MultiplicationTable
* 感觉非常变态的题，完全无法用常理推断
* 因为 ((a,b),c) == (a,(b,c))，那么常规的从(0,0)往后推导不难，设 f(i) = (0,...(0,0)) 且 |0| == i
* 当求(i,j)，i != (0,...(0,0))时，如果(0,i) = f(n)，就可以假设 i = f(n-1)

### FoxesOfTheRoundTable
* 感觉十分trick的一题
* 想到过用贪心，不过还是枚举+贪心双保险

### ExactTree
* 将求任意两点间的最短路径和看做对于每条边，求将其两端的点数积
