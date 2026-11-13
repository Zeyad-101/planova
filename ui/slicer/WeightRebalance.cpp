#include "slicer/WeightRebalance.h"
#include <algorithm>
#include <cmath>

namespace planova::slicer {

ScoringWeights rebalance(ScoringWeights current, int changedIndex, int newValue) {
    newValue = std::clamp(newValue, 0, 100);
    const int remainder = 100 - newValue;

    int a = 0, b = 0;  // the two sliders that were NOT moved, in field order
    if (changedIndex == 0) { a = current.workload; b = current.priority; }
    else if (changedIndex == 1) { a = current.skill; b = current.priority; }
    else { a = current.skill; b = current.workload; }

    int newA = 0;
    if (a + b == 0) newA = remainder / 2;
    else newA = static_cast<int>(std::lround(static_cast<double>(remainder) * a / (a + b)));
    const int newB = remainder - newA;  // guarantees the sum is exactly 100

    ScoringWeights r;
    if (changedIndex == 0) { r.skill = newValue; r.workload = newA; r.priority = newB; }
    else if (changedIndex == 1) { r.workload = newValue; r.skill = newA; r.priority = newB; }
    else { r.priority = newValue; r.skill = newA; r.workload = newB; }
    return r;
}

} // namespace planova::slicer
