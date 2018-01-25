# coding=utf-8
import math


def check(n):
	s = str(n)
	if len(s) != 9:
		return False
	p = [1, 0, 0, 0, 0, 0, 0, 0, 0, 0]
	for c in s:
		_c = ord(c) - 48
		if p[_c]:
			return False
		p[_c] = 1
	return True


def f():
	a, b = 1, 1
	n = 2
	mod = 10 ** 9
	g = math.log10((1 + math.sqrt(5.0)) / 2.0)
	o = math.log10(5.0) / 2
	while True:
		a, b = (a + b) % mod, a
		n += 1
		if not check(a):
			continue
		t = (n * g - o)
		if check(long(math.pow(10, t - int(t) + 8))):
			return n


print f()
