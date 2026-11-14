#pragma once
#include "planova/slicer/ScoringWeights.h"

namespace planova::slicer {

// index: 0 = skill, 1 = workload, 2 = priority. The other two are rescaled so the sum is exactly 100.
ScoringWeights rebalance(ScoringWeights current, int changedIndex, int newValue);

} // namespace planova::slicer
