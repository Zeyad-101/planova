#pragma once
#include "planova/common/Queries.h"
#include <optional>
#include <vector>

class FakeMemberQuery : public IMemberQuery {
public:
    void addMember(const MemberDto& m) { members_.push_back(m); }
    std::vector<MemberDto> all() const override { return members_; }
    std::optional<MemberDto> byId(const Id& id) const override {
        for (const auto& m : members_) if (m.id == id) return m;
        return std::nullopt;
    }
private:
    std::vector<MemberDto> members_;
};
