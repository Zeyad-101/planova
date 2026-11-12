#pragma once
#include "planova/common/Types.h"
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

struct TaskDto {
    Id id;
    std::string title;
    SkillCategory skill;
    Priority priority;
    int complexity;
    int estHours;
    TaskStatus status;
    std::optional<Id> assignee;
    std::vector<Id> blockedBy;
};

struct MemberDto {
    Id id;
    std::string name;
    std::string role;
    bool isLead;
    std::map<SkillCategory, int> ratings;
    int understanding;
    double velocity;
};

struct ProjectDto {
    Id id;
    std::string name;
    std::string team;
    std::string seModel;
};

struct PinDto {
    Id id;
    PinCategory category;
    std::string text;
    std::vector<std::string> tags;
    Id authorId;
    int64_t ts;
};

struct CommitDto {
    std::string hash;
    std::string message;
    std::string author;
    int64_t ts;
};

struct IssueDto {
    int number;
    std::string title;
    bool open;
};

struct NewTask {
    std::string title;
    SkillCategory skill;
    Priority priority;
    int complexity;
    int estHours;
    std::vector<Id> deps;
};