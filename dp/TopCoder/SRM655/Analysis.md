### https://community.topcoder.com/stat?c=round_overview&rd=16415
### http://apps.topcoder.com/wiki/display/tc/SRM+655

### Nine
* 如果数能被9整除当且仅当其各个十进制位的和能被9整除
* 将占用相同问题的数位联合起来考虑
* dp过程中枚举状态没有先后关系
* dp(mask,remainders) -> remainder_of_mask -> dp(next_mask,(remainders+remainder_of_mask)%9)