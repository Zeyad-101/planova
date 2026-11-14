#pragma once
#include "planova/common/Types.h"
#include <string>

namespace planova::slicer {

// Note: TaskAssigned belongs to S3's event block (section 2 of team plan):
// same string, payload {taskId, memberId, by}.
inline constexpr const char* TaskAssigned = "TaskAssigned";

// SlicerWeightsChanged belongs to S4:
// payload {skill, workload, priority} (integers summing to 100).
inline constexpr const char* SlicerWeightsChanged = "SlicerWeightsChanged";

inline std::string skillToString(SkillCategory s) {
    switch (s) {
    case SkillCategory::Backend: return "Backend";
    case SkillCategory::Frontend: return "Frontend";
    case SkillCategory::Storage: return "Storage";
    case SkillCategory::UiUx: return "UiUx";
    case SkillCategory::Integration: return "Integration";
    case SkillCategory::Testing: return "Testing";
    case SkillCategory::Documentation: return "Documentation";
    }
    return "Unknown";
}

inline std::string priorityToString(Priority p) {
    switch (p) {
    case Priority::Low: return "Low";
    case Priority::Medium: return "Medium";
    case Priority::High: return "High";
    case Priority::Critical: return "Critical";
    }
    return "Unknown";
}

} // namespace planova::slicer
