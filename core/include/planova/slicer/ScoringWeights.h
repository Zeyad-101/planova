#pragma once

namespace planova::slicer {

struct ScoringWeights {
    int skill = 50;
    int workload = 30;
    int priority = 20;

    bool valid() const noexcept {
        return skill + workload + priority == 100 && skill >= 0 && workload >= 0 && priority >= 0;
    }
    bool operator==(const ScoringWeights& o) const noexcept {
        return skill == o.skill && workload == o.workload && priority == o.priority;
    }
    bool operator!=(const ScoringWeights& o) const noexcept { return !(*this == o); }
};

} // namespace planova::slicer
