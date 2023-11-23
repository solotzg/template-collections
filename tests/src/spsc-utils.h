
namespace tests {

struct TestNode {
  TestNode(const TestNode &) = delete;
  TestNode(int g) : v(g) { inc_cnt(); }
  ~TestNode() { dec_cnt(); }
  TestNode(TestNode &&src) {
    inc_cnt();
    v = src.v;
  }
  TestNode &operator=(TestNode &&src) {
    inc_cnt();
    v = src.v;
    return *this;
  }
  static std::atomic<int> &test_node_cnt() {
    static std::atomic<int> data{};
    return data;
  }
  static void inc_cnt() { test_node_cnt()++; }
  static void dec_cnt() { test_node_cnt()--; }
  int v{};
};
} // namespace tests