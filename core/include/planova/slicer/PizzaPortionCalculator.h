#pragma once
#include "planova/common/Dtos.h"
#include "planova/slicer/SlicerTypes.h"
#include <vector>

namespace planova::slicer {

class PizzaPortionCalculator {
public:
    // Counts every task that has an assignee (any status, Done included).
    static std::vector<Portion> calculate(const std::vector<TaskDto>& tasks,
                                          const std::vector<MemberDto>& members);
};

} // namespace planova::slicer
