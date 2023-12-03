#pragma once

#include <utils/utils.h>

struct EulerFunction : utils::noncopyable {
  using Vec = std::vector<int>;

  EulerFunction(size_t maxn) { init(maxn); }
  const Vec &phi() const { return phi_; }
  Vec::value_type phi(size_t index) const { return phi()[index]; }
  const Vec &prime() const { return prime_; }

private:
  void init(size_t maxn) {
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

private:
  Vec phi_;
  Vec prime_;
};
