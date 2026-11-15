#include "planova/team/MemberProjection.h"
#include "planova/common/EventTypes.h"
#include "planova/team/SkillCategory.h"
#include "planova/team/VelocityCalculator.h"

#include <QJsonObject>
#include <QString>

using namespace std;

void MemberProjection::reset()
{
    members_.clear();
    memberOrder_.clear();
    settings_.clear();
    taskInProgressTs_.clear();
}

void MemberProjection::apply(const Event& event)
{
    const QJsonObject& p = event.payload;

    if (event.type == MemberAdded) {
        Id id = p["memberId"].toString().toStdString();
        if (id.empty()) {
            return;
        }

        Member m;
        m.id = id;
        m.name = p["name"].toString().toStdString();
        m.role = p["role"].toString().toStdString();
        m.isLead = p["isLead"].toBool(false);
        m.understanding = 0;
        m.velocity = 1.0;

        if (members_.find(id) == members_.end()) {
            memberOrder_.push_back(id);
        }
        members_[id] = m;
    }
    else if (event.type == MemberUpdated) {
        Id id = p["memberId"].toString().toStdString();
        auto it = members_.find(id);
        if (it != members_.end()) {
            it->second.role = p["role"].toString().toStdString();
            it->second.understanding = p["understanding"].toInt(0);
        }
    }
    else if (event.type == SkillRated) {
        Id id = p["memberId"].toString().toStdString();
        auto it = members_.find(id);
        if (it != members_.end()) {
            string catStr = p["category"].toString().toStdString();
            SkillCategory cat = fromString(catStr);
            int rating = p["rating"].toInt(1);
            it->second.ratings[cat] = rating;
        }
    }
    else if (event.type == SettingChanged) {
        string key = p["key"].toString().toStdString();
        bool val = p["value"].toBool(false);
        settings_[key] = val;
    }
else if (event.type == "TaskMoved") {
        Id taskId = p["taskId"].toString().toStdString();
        string toStatus = p["toStatus"].toString().toStdString();
        if (toStatus.empty()) {
            toStatus = p["status"].toString().toStdString();
        }

        if (toStatus == "InProgress" && !taskId.empty()) {
            taskInProgressTs_[taskId] = event.ts;
        }
        else if (toStatus == "Done") {
            Id memberId = p["assigneeId"].toString().toStdString();
            if (memberId.empty()) {
                memberId = p["memberId"].toString().toStdString();
            }

            auto memIt = members_.find(memberId);
            if (memIt != members_.end()) {
                double estimatedHours = p["estimatedHours"].toDouble(0.0);
                double actualHours = p["actualHours"].toDouble(0.0);

                if (actualHours <= 0.0 && !taskId.empty()) {
                    auto tsIt = taskInProgressTs_.find(taskId);
                    if (tsIt != taskInProgressTs_.end() && event.ts > tsIt->second) {
                        actualHours = static_cast<double>(event.ts - tsIt->second) / 3600000.0;
                    }
                }

                if (estimatedHours > 0.0 && actualHours > 0.0) {
                    memIt->second.velocity = VelocityCalculator::update(
                        memIt->second.velocity,
                        estimatedHours,
                        actualHours
                        );
                }
            }
        }
    }
}

vector<MemberDto> MemberProjection::all() const
{
    vector<MemberDto> result;
    result.reserve(memberOrder_.size());

    for (const Id& id : memberOrder_) {
        auto it = members_.find(id);
        if (it != members_.end()) {
            result.push_back(it->second.toDto());
        }
    }
    return result;
}

optional<MemberDto> MemberProjection::byId(const Id& id) const
{
    auto it = members_.find(id);
    if (it != members_.end()) {
        return it->second.toDto();
    }
    return nullopt;
}

bool MemberProjection::getSetting(const string& key, bool defaultValue) const
{
    auto it = settings_.find(key);
    if (it != settings_.end()) {
        return it->second;
    }
    return defaultValue;
}