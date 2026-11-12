#include "planova/project/ProjectService.h"
#include "planova/common/EventTypes.h"
#include "planova/common/Ids.h"

#include <QJsonObject>
#include <QString>

ProjectService::ProjectService(EventLog& log, IClock& clock)
    : log_(log), clock_(clock)
{
    log_.add(projection_);
}

Id ProjectService::create(const std::string& name, const std::string& team,
                          const std::string& seModel, const std::string& idea)
{
    const int64_t ts = clock_.nowMs();

    QJsonObject payload;
    payload["name"] = QString::fromStdString(name);
    payload["teamName"] = QString::fromStdString(team);
    payload["seModel"] = QString::fromStdString(seModel);
    payload["ideaText"] = QString::fromStdString(idea);

    const Id projectId = makeEventId(ts, team, payload);
    payload["projectId"] = QString::fromStdString(projectId);

    Event event;
    event.id = makeEventId(ts, team, payload);
    event.ts = ts;
    event.author = team;
    event.type = ProjectCreated;
    event.payload = payload;
    log_.append(event);

    return projectId;
}

std::vector<ProjectDto> ProjectService::list() const
{
    return projection_.list();
}