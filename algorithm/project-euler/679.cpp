// Aho-Corasick Algorithm

#include "utils/utils.h"

constexpr U64 kCharNum = 4;
constexpr U64 kNodeNum = 16;

class Tire {
  struct TireNode;

public:
  Tire() {
    memset(mmp_, -1, sizeof(mmp_));
    mmp_['A'] = 0;
    mmp_['E'] = 1;
    mmp_['F'] = 2;
    mmp_['R'] = 3;

    root_ = NewNode();
    root_->init(root_);
  }

  void Add(std::string_view sv) {
    auto *node = root_;
    for (auto c : sv) {
      auto x = mmp_[c];
      if (!IsValid(node->next[x])) {
        node->next[x] = NewNode();
      }
      node = node->next[x];
    }
    node->key_index = key_cnt_++;
  }
  void BuildFail() {
    TireNode *queue[kNodeNum];
    size_t bg = 0, ed = 0;
    {
      auto *node = root_;
      rp(x, kCharNum) {
        auto next = node->next[x];
        if (IsValid(next)) {
          next->fail = root_;
          queue[ed++] = next;
        }
      }
    }
    while (bg != ed) {
      auto *node = queue[bg++];
      rp(x, kCharNum) {
        auto next = node->next[x];
        if (!IsValid(next)) {
          node->next[x] = node->fail->next[x];
          continue;
        }
        next->fail = node->fail->next[x];
        queue[ed++] = next;
      }
    }
    ASSERT_EQ(node_cnt_, kNodeNum);
    ASSERT_EQ(key_cnt_, kCharNum);
  }
  bool Test(std::string_view sv) {
    std::array<bool, kCharNum> k{};
    auto *node = root_;
    for (auto c : sv) {
      auto x = mmp_[c];
      node = node->next[x];
      if (node->key_index != -1) {
        if (k[node->key_index])
          return false;
        k[node->key_index] = true;
      }
    }
    return std::all_of(k.begin(), k.end(), [](const auto b) { return b; });
  }
  int ToIndex(const TireNode *node) const { return node - root_; }
  const TireNode *root() const { return root_; };
  const TireNode *node(size_t i) const { return &nodes_[i]; }

private:
  struct TireNode {
    TireNode *next[kCharNum]{};
    TireNode *fail{};
    int key_index{-1};

    void init(TireNode *root) {
      fail = root;
      rp(i, kCharNum) next[i] = root;
    }
  };
  bool IsValid(TireNode *node) const { return node != root_; }

  TireNode *NewNode() {
    auto node = &nodes_[node_cnt_++];
    node->init(root_);
    return node;
  }

private:
  int mmp_['Z' + 1];
  std::array<TireNode, kNodeNum> nodes_;
  int node_cnt_{};
  int key_cnt_ = 0;
  TireNode *root_{};
};

class DP {
public:
  static constexpr U64 kMaxMask = 1 << kCharNum;
  U64 Calc(U64 maxn, const Tire &tire) {
    std::memset(data_, 0, sizeof(data_));
    rp(x, kCharNum) {
      auto p = tire.root()->next[x];
      data_[1][tire.ToIndex(p)][0] += 1;
    }
    repd(n, 2, maxn) {
      rp(i, kNodeNum) {
        rp(mask, kMaxMask) {
          rp(x, kCharNum) {
            auto next = tire.node(i)->next[x];
            if (auto index = next->key_index; -1 != index) {
              if (mask & (1 << index)) {
              } else {
                data_[n][tire.ToIndex(next)][mask + (1 << index)] +=
                    data_[n - 1][i][mask];
              }
            } else {
              data_[n][tire.ToIndex(next)][mask] += data_[n - 1][i][mask];
            }
          }
        }
      }
    }
    U64 res = 0;
    rp(i, kNodeNum) { res += data_[maxn][i][kMaxMask - 1]; }
    return res;
  }

private:
  U64 data_[31][kNodeNum][kMaxMask];
};

int main() {
  Tire tire{};
  {
    for (std::string_view sv : {"FREE", "FARE", "AREA", "REEF"}) {
      tire.Add(sv);
    }
    tire.BuildFail();
    ASSERT(tire.Test("FREEFAREA"));
    ASSERT(!tire.Test("FREEFAREAREA"));
  }
  DP dp{};
  DEBUG_SCOPE({
    auto res = dp.Calc(9, tire);
    ASSERT_EQ(res, 1);
  });
  DEBUG_SCOPE({
    auto res = dp.Calc(15, tire);
    ASSERT_EQ(res, 72863);
  });
  MSGLN(dp.Calc(30, tire));

  return 0;
}