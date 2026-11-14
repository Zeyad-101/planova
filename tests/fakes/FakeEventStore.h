#pragma once
#include "planova/common/IEventStore.h"
#include <algorithm>
#include <vector>

class FakeEventStore : public IEventStore {
public:
    void append(const Event& e) override { events.push_back(e); }
    std::vector<Event> readAll() const override {
        auto copy = events;
        std::sort(copy.begin(), copy.end(), [](const Event& a, const Event& b) {
            return a.ts != b.ts ? a.ts < b.ts : a.id < b.id;
        });
        return copy;
    }
    std::vector<Event> events;
};
