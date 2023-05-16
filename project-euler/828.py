from itertools import permutations
import time


data = dict()


def foo(res, u, v):
    if u <= 0:
        return
    if u not in res:
        res[u] = v
    else:
        res[u] = min(res[u], v)


def dfs(nums, bg, ed):
    x = data.get((bg, ed))
    if x:
        return x
    if bg + 1 == ed:
        res = {nums[bg]: nums[bg], 0: 0}
        data[(bg, ed)] = res
        return res
    res = {0: 0}
    for p in range(bg+1, ed):
        a = dfs(nums, bg, p)
        b = dfs(nums, p, ed)
        for x, v1 in a.items():
            for y, v2 in b.items():
                foo(res, x+y, v1+v2)
                foo(res, x-y, v1+v2)
                foo(res, x*y, v1+v2)
                if y != 0 and x % y == 0:
                    foo(res, x//y, v1+v2)

    data[(bg, ed)] = res
    return res


def wrap_run_time(func):
    def wrap_func(*args, **kwargs):
        bg = time.time()
        r = func(*args, **kwargs)
        print('Time cost {:.3f}s'.format(time.time() - bg))
        return r

    return wrap_func


@wrap_run_time
def main():
    n = 1
    ans = 0
    with open("/tmp/p828_number_challenges.txt") as f:
        for line in f.readlines():
            line = line.strip()
            if not line:
                continue
            tar, x = line.split(':')
            nums = [int(a) for a in x.split(',')]
            nums.sort()
            tar = int(tar)
            p = None
            for pnums in permutations(nums):
                data.clear()
                res = dfs(pnums, 0, len(pnums))
                if tar in res:
                    if p is None:
                        p = res[tar]
                    else:
                        p = min(p, res[tar])
            p = 0 if p is None else p
            print(n, p)
            ans += pow(3, n, 1005075251) * p % 1005075251
            ans %= 1005075251
            n += 1
    print("ans:", ans)


if __name__ == "__main__":
    main()
