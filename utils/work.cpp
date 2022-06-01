#include "work.h"

TimeCost::TimeCost(const char *label)
    : label_(label), start_(std::chrono::system_clock::now()) {}

TimeCost::~TimeCost() {
  auto end = std::chrono::system_clock::now();

  MSGLN(label_ << " time cost: "
               << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                        start_)
                      .count()
               << "ms");
}