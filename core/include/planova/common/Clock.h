#pragma once
#include <cstdint>

class IClock {
public:
    virtual ~IClock() = default;
    virtual int64_t nowMs() const = 0;
};