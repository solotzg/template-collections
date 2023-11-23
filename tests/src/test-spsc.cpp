#include "tests/tests.h"
#include "utils/spsc_queue.hpp"

namespace tests {

using TestNode = TestNode<int>;

static void _test_spsc_queue() {
  {
    ASSERT_EQ(utils::NextPow2(1), 1);
    ASSERT_EQ(utils::NextPow2(2), 2);
    for (uint32_t i = 3; i <= 4; ++i)
      ASSERT_EQ(utils::NextPow2(i), 4);
    for (uint32_t i = 5; i <= 8; ++i)
      ASSERT_EQ(utils::NextPow2(i), 8);
    for (uint32_t i = 9; i <= 16; ++i)
      ASSERT_EQ(utils::NextPow2(i), 16);
    ASSERT_EQ(utils::NextPow2(1u << 31), 1u << 31);
  }
  static_assert(alignof(utils::SPSCQueue<void *>) == 64);
  {
    utils::SPSCQueue<uint8_t> s(1);
    auto producer = s.GenProducer();
    auto customer = s.GenCustomer();

    assert(producer.Put(88));
    assert(customer.Get().value() == 88);
  }
  {
    utils::SPSCQueue<std::string> s(13);
    auto producer = s.GenProducer();
    auto customer = s.GenCustomer();

    assert(s.capacity() == 15);
    {
      std::string ori = "12345";
      assert(producer.Put(std::string(ori)));
      auto v = customer.Get();
      assert(v);
      assert((*v) == ori);
      assert(!customer.Get());
    }
  }
  {
    utils::SPSCQueue<std::string> s(3);
    auto producer = s.GenProducer();
    auto customer = s.GenCustomer();

    assert(s.capacity() == 3);
    for (int i = 0; i < 3; ++i) {
      assert(producer.Put(std::to_string(i)));
    }
    assert(!producer.Put("..."));
    std::string v;
    assert(customer.Get(v));
    assert(v == "0");
    assert(customer.Get(v));
    assert(v == "1");
    assert(customer.Get(v));
    assert(v == "2");
    {
      assert(producer.Put("..."));
      assert(customer.Get() == "...");
    }
  }
  {
    {
      TestNode _{1};
      assert(TestNode::obj_cnt() == 1);
    }
    ASSERT_EQ(TestNode::obj_cnt(), 0);
    {
      utils::SPSCQueue<TestNode> s(3);
      auto producer = s.GenProducer();
      auto customer = s.GenCustomer();

      producer.Put(TestNode{2});
      producer.Put(TestNode{3});
      producer.Put(TestNode{4});
      assert(TestNode::obj_cnt() == 3);
      customer.Get();
      assert(TestNode::obj_cnt() == 2);
    }
    ASSERT_EQ(TestNode::obj_cnt(), 0);

    {
      utils::SPSCQueue<TestNode> s(3);
      auto producer = s.GenProducer();
      auto customer = s.GenCustomer();

      std::thread t1([&]() {
        for (int i = 0; i < 8192; ++i) {
          while (!producer.Put(TestNode{i + 1}))
            std::this_thread::yield();
        }
      });
      for (int i = 0; i < 8192; ++i) {
        while (1) {
          auto v = customer.Get();
          if (v) {
            assert(v->value() == i + 1);
            break;
          } else
            std::this_thread::yield();
        }
      }
      t1.join();
      ASSERT_EQ(TestNode::obj_cnt(), 0);
    }

    {
      utils::Notifier customer_notifier;

      utils::SPSCQueueWithNotifer<utils::SPSCQueue<TestNode>> s{3};

      std::thread t1([&]() {
        for (int i = 0; i < 8192; ++i) {
          ASSERT(s.Put(TestNode{i + 1}, utils::Seconds{8192},
                       [&] { customer_notifier.Wake(); }));
          customer_notifier.Wake();
        }
      });
      for (int i = 0; i < 8192; ++i) {
        auto v = s.Get(utils::Seconds{8192}, customer_notifier,
                       [&] { s.WakeProducer(); });
        ASSERT(v);
        ASSERT_EQ(v->value(), i + 1);
        s.WakeProducer();
      }
      t1.join();
      ASSERT_EQ(TestNode::obj_cnt(), 0);
    }
  }
}
} // namespace tests

template <>
struct fmt::formatter<tests::TestNode> : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const tests::TestNode &a, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(fmt::format("{}", a.value()),
                                               ctx);
  }
};

FUNC_FACTORY_REGISTER("spsc", tests::_test_spsc_queue);
