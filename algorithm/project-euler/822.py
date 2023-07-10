# phi(MOD) = MOD-1
# gcd(x, MOD) = 1 ->  x^(phi(MOD)) % MOD = 1
# 2^y = phi(MOD) * a + b
# x^(2^y) % MOD = (x^phi(MOD))^a * x^b % MOD = x^b % MOD = x^(2^y % (MOD-1)) % MOD

def solve(n, m):
    MOD = 1234567891
    data = [i for i in range(2, n+1)]
    step = 0
    for _ in range(m):
        small = data[0]**2
        over = 0
        if small > data[-1]:
            over = 1
        data[0] = small
        data = sorted(data)
        step += 1
        if over:
            break
    if step < m:
        cnt = m - step
        times = cnt // len(data)
        remain = cnt % len(data)
        new_data = []
        for e in data:
            # https://en.wikipedia.org/wiki/Euler%27s_theorem
            new_data.append(pow(e, pow(2, times, MOD-1), MOD))
        for i in range(remain):
            new_data[i] = new_data[i]**2 % MOD
        data = new_data

    res = 0
    for e in data:
        res = (res+e) % MOD
    return "S({},{})={}".format(n, m, res)


def main():
    print(solve(10, 100))
    print(solve(10**4, 10**16))


if __name__ == "__main__":
    main()
