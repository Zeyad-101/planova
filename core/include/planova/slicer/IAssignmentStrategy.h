#pragma once
#include "planova/common/Dtos.h"
#include "planova/slicer/ScoringWeights.h"
#include "planova/slicer/SlicerTypes.h"
#include <vector>

namespace planova::slicer {

class IAssignmentStrategy {
public:
    virtual ~IAssignmentStrategy() = default;
    virtual std::vector<Assignment> assign(const std::vector<TaskDto>& tasks,
                                           const std::vector<MemberDto>& members,
                                           const ScoringWeights& weights) const = 0;
};

} // namespace planova::slicer
