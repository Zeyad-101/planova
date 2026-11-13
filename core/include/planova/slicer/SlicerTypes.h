#pragma once
#include "planova/common/Types.h"
#include <string>

namespace planova::slicer {

struct Assignment {
    Id taskId;
    Id memberId;
    int matchPct = 0;
    std::string rationale;
    bool waitingOnDep = false;
};

struct Portion {
    Id memberId;
    std::string memberName;
    int taskCount = 0;
    int totalComplexity = 0;
    double share = 0.0;      // 0.0 to 1.0
    double startAngle = 0.0; // degrees, clockwise from 12 o'clock
    double spanAngle = 0.0;
    bool thinSlice = false;  // true when the member has no assigned complexity
};

} // namespace planova::slicer
