#pragma once
#include "planova/common/Clock.h"

class FakeClock : public IClock {
public:
    explicit FakeClock(int64_t ms = 0) : ms_(ms) {}
    int64_t nowMs() const override { return ms_; }
    void setTime(int64_t ms) { ms_ = ms; }
private:
    int64_t ms_;
};
