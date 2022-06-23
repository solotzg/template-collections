#include "work.h"
#include <chrono>

TimeCost::TimeCost(const char *label)
    : label_(label), start_(std::chrono::steady_clock::now()) {}

void TimeCost::Show(const char *prefix) const {
  auto end = std::chrono::steady_clock::now();

  MSG("[" << label_ << "]");
  if (prefix)
    MSG("[" << prefix << "]");
  MSGLN("[time cost: " << std::chrono::duration_cast<std::chrono::milliseconds>(
                              end - start_)
                              .count()
                       << "ms]");
}

TimeCost::~TimeCost() { Show("END"); }

void TimeCost::Reset() { start_ = std::chrono::steady_clock::now(); }
