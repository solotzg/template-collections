# -*- coding: utf-8 -*-
"""

"""

h = dict()
m = dict()
mmp = set()
n = 1500000
ans = [0] * (n + 1)
for i in xrange(1, n + 1):
	j = i * i
	h[j] = i
	m[i] = j
for i in xrange(1, n):
	for j in xrange(i + 1, n):
		c = i * i + j * j
		b = i * j * 2
		a = j * j - i * i
		o = a + b + c
		if a + b + c > n:
			break
		if b < a:
			a, b = b, a
		for k in xrange(1, 1+n):
			if o * k <= n:
				p = (k*a, k*b, k*c)
				if p not in mmp:
					ans[o * k] += 1
					mmp.add(p)
			else:
				break
p = 0
for e in ans:
	if e == 1:
		p += 1
print p
