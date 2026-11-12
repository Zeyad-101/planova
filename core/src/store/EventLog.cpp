#include "planova/store/EventLog.h"

EventLog::EventLog(IEventStore& store)
    : store_(store)
{
}

void EventLog::add(IProjection& projection)
{
    projections_.push_back(&projection);
}

void EventLog::rebuild()
{
    for (auto* p : projections_)
        p->reset();
    for (const auto& e : store_.readAll())
        for (auto* p : projections_)
            p->apply(e);
}

void EventLog::append(const Event& event)
{
    store_.append(event);
    for (auto* p : projections_)
        p->apply(event);
    bus_.publish(event.type);
}

ChangeBus& EventLog::bus()
{
    return bus_;
}