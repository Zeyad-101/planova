#pragma once

#include <string>
#include <stdexcept>
#include "planova/common/Types.h"

inline std::string toString(SkillCategory category) {
    switch (category) {
    case SkillCategory::Backend:       return "Backend";
    case SkillCategory::Frontend:      return "Frontend";
    case SkillCategory::Storage:       return "Storage";
    case SkillCategory::UiUx:          return "UiUx";
    case SkillCategory::Integration:   return "Integration";
    case SkillCategory::Testing:       return "Testing";
    case SkillCategory::Documentation: return "Documentation";
    }
    throw std::invalid_argument("Invalid SkillCategory");
}

inline SkillCategory fromString(const std::string& str) {
    if (str == "Backend")       return SkillCategory::Backend;
    if (str == "Frontend")      return SkillCategory::Frontend;
    if (str == "Storage")       return SkillCategory::Storage;
    if (str == "UiUx")          return SkillCategory::UiUx;
    if (str == "Integration")   return SkillCategory::Integration;
    if (str == "Testing")       return SkillCategory::Testing;
    if (str == "Documentation") return SkillCategory::Documentation;

    throw std::invalid_argument("Unknown SkillCategory: " + str);
}