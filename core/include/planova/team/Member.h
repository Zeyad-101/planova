#pragma once

#include <string>
#include <map>
#include <algorithm>
#include "planova/common/Types.h"
#include "planova/common/Dtos.h"

class Member {
public:
    Id id;
    std::string name;
    std::string role;
    bool isLead = false;
    std::map<SkillCategory, int> ratings;
    int understanding = 0;
    double velocity = 1.0;

    Member() = default;

    Member(const Id& memberId, const std::string& memberName, const std::string& memberRole, bool lead)
        : id(memberId), name(memberName), role(memberRole), isLead(lead) {}

    void setUnderstanding(int value) {
        understanding = std::clamp(value, 0, 100); // Clamps 0..100 automatically!
    }

    void setRating(SkillCategory category, int rating) {
        ratings[category] = rating;
    }

    MemberDto toDto() const {
        return MemberDto{id, name, role, isLead, ratings, understanding, velocity};
    }
};