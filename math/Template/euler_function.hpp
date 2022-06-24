#pragma once

#include <cassert>
#include <vector>

struct EulerFunction {
  void init(size_t maxn = 100) {
    phi_.resize(maxn + 1, 0);
    prime_.reserve(std::sqrt(maxn));
    std::vector<bool> vis;
    vis.resize(maxn + 1, false);

    phi_[1] = 1;
    repd(i, 2, maxn) {
      if (!vis[i]) {
        prime_.emplace_back(i);
        phi_[i] = i - 1;
      }
      for (int j = 0, tp; j < prime_.size() && LL(prime_[j]) * i <= maxn; ++j) {
        tp = prime_[j] * i;
        vis[tp] = 1;
        if (i % prime_[j] == 0) {
          phi_[tp] = prime_[j] * phi_[i];
          break;
        } else {
          phi_[tp] = phi_[i] * (prime_[j] - 1);
        }
      }
    }
  }
  std::vector<int> &phi() { return phi_; }
  std::vector<int> &prime() { return prime_; }

private:
  std::vector<int> phi_, prime_;
};

#ifndef NDEBUG
static void _test_euler_function() {
  EulerFunction euler_function;
  euler_function.init(10);
  assert((euler_function.prime() == std::vector{2, 3, 5, 7}));
  std::vector<int> d;
  for (int x = 7; x != 1;) {
    d.emplace_back(x);
    x = euler_function.phi()[x];
  }
  d.emplace_back(1);
  assert((d == std::vector{7, 6, 2, 1}));
}
#endif