#pragma once
#include "planova/common/Event.h"

class IProjection {
public:
    virtual ~IProjection() = default;
    virtual void reset() = 0;
    virtual void apply(const Event&) = 0;
};