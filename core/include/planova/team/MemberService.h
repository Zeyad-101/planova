#pragma once

#include <string>
#include "planova/common/Clock.h"
#include "planova/common/Types.h"
#include "planova/store/EventLog.h"

class MemberService {
public:
    MemberService(EventLog& log, IClock& clock);

    Id add(const std::string& name, const std::string& role, bool isLead);
    void update(const Id& id, const std::string& role, int understanding);
    void rate(const Id& id, SkillCategory category, int rating);
    void setSetting(const std::string& key, bool value);

private:
    EventLog& log_;
    IClock& clock_;
};