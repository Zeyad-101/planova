#pragma once

#include <map>
#include <string>
#include <vector>
#include <optional>
#include "planova/common/Types.h"
#include "planova/common/Dtos.h"
#include "planova/common/Event.h"
#include "planova/common/IProjection.h"
#include "planova/common/Queries.h"
#include "planova/team/Member.h"

class MemberProjection : public IProjection, public IMemberQuery {
public:
    MemberProjection() = default;

    void reset() override;
    void apply(const Event& event) override;

    std::vector<MemberDto> all() const override;
    std::optional<MemberDto> byId(const Id& id) const override;

    bool getSetting(const std::string& key, bool defaultValue = false) const;

private:
    std::map<Id, Member> members_;
    std::vector<Id> memberOrder_;
    std::map<std::string, bool> settings_;
    std::map<Id, int64_t> taskInProgressTs_;
};