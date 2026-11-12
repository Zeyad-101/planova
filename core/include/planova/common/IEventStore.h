#pragma once
#include "planova/common/Event.h"
#include <vector>

class IEventStore {
public:
    virtual ~IEventStore() = default;
    virtual void append(const Event&) = 0;
    virtual std::vector<Event> readAll() const = 0;
};