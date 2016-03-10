### https://community.topcoder.com/stat?c=round_overview&rd=16313
### http://apps.topcoder.com/wiki/display/tc/SRM+649

### Decipherability
* dp考察基本功，没什么奇怪的地方

### XorSequence
* div2 相同问题的升级版
* 其中一个子问题为在一个数组 A 中找出 i< j 且 A[i] < A[j] 的对数，经典老问题了，可参考merge排序
* 设数组的二进制为 10, 01, 11, 10, 00
* 第2位的数选0或1后，最后变成 00, 11, 01, 00, 10 或 10, 01, 11, 10, 00
* 若最终选定第二位为0，则第一位不受影响
