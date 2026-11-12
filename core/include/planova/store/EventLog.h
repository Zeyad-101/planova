#pragma once
#include "planova/common/ChangeBus.h"
#include "planova/common/Event.h"
#include "planova/common/IEventStore.h"
#include "planova/common/IProjection.h"
#include <vector>

class EventLog {
public:
    explicit EventLog(IEventStore& store);
    void add(IProjection& projection);
    void rebuild();
    void append(const Event& event);
    ChangeBus& bus();

private:
    IEventStore& store_;
    std::vector<IProjection*> projections_;
    ChangeBus bus_;
};