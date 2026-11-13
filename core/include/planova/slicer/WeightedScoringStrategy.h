#pragma once
#include "planova/slicer/IAssignmentStrategy.h"

namespace planova::slicer {

class WeightedScoringStrategy : public IAssignmentStrategy {
public:
    std::vector<Assignment> assign(const std::vector<TaskDto>& tasks,
                                   const std::vector<MemberDto>& members,
                                   const ScoringWeights& weights) const override;
};

} // namespace planova::slicer
