#include "work.h"
#include <chrono>

TimeCost::TimeCost(const char *label) : label_(label), start_(Clock::now()) {}

void TimeCost::Show(const char *prefix) const {
  auto end = Clock::now();

  MSG("[" << label_ << "]");
  if (prefix)
    MSG("[" << prefix << "]");
  MSGLN("[time cost: " << std::chrono::duration_cast<std::chrono::milliseconds>(
                              end - start_)
                              .count()
                       << "ms]");
}

TimeCost::~TimeCost() { Show("END"); }

void TimeCost::Reset() { start_ = Clock::now(); }

TimeCost::Clock::duration TimeCost::Duration() {
  return start_.time_since_epoch();
}
