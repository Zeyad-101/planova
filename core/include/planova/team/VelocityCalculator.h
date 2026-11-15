#pragma once

#include <algorithm>

class VelocityCalculator {
public:
    static constexpr double kMinVelocity = 0.5;
    static constexpr double kMaxVelocity = 2.0;
    static constexpr double kDefaultAlpha = 0.3;

    // Calculates updated velocity using Exponential Moving Average (EMA)
    // clamped to [0.5, 2.0].
    static double update(double currentVelocity,
                         double estimatedHours,
                         double actualHours,
                         double alpha = kDefaultAlpha)
    {
        if (estimatedHours <= 0.0 || actualHours <= 0.0) {
            return std::clamp(currentVelocity, kMinVelocity, kMaxVelocity);
        }

        const double taskRatio = estimatedHours / actualHours;
        const double blended = (1.0 - alpha) * currentVelocity + alpha * taskRatio;
        return std::clamp(blended, kMinVelocity, kMaxVelocity);
    }
};