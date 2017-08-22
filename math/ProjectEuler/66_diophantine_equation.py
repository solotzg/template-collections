import math

N = 1000
res = -1
resn = None
for n in xrange(1, N+1):
	a = int(math.sqrt(n))
	if a * a == n:
		continue
	c = _c = a
	d = _d = 1
	ans = [a]
	while True:
		x, y = 1, 0
		for i in xrange(len(ans) - 1, -1, -1):
			x, y = y, x
			x = x + ans[i] * y
		if x * x - n * y * y == 1:
			if x > res:
				res = x
				resn = n
			break
		o = n - c * c
		o /= d
		k = (a + c)/o
		c = o * k - c
		d = o
		ans.append(k)
print resn
