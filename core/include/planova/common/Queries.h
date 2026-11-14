#pragma once

#include "planova/common/Dtos.h"
#include "planova/common/Types.h"
#include <optional>
#include <vector>

struct ITaskQuery {
    virtual ~ITaskQuery() = default;
    virtual std::vector<TaskDto> all() const = 0;
    virtual std::optional<TaskDto> byId(const Id&) const = 0;
};

struct IMemberQuery {
    virtual ~IMemberQuery() = default;
    virtual std::vector<MemberDto> all() const = 0;
    virtual std::optional<MemberDto> byId(const Id&) const = 0;
};

struct IPinQuery {
    virtual ~IPinQuery() = default;
    virtual int openCount() const = 0;
    virtual std::vector<PinDto> latest(int n) const = 0;
};

struct IConflictQuery {
    virtual ~IConflictQuery() = default;
    virtual bool hasOpenConflict(const Id& memberId) const = 0;
};
