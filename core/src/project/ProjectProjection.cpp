#include "planova/project/ProjectProjection.h"
#include "planova/common/EventTypes.h"

void ProjectProjection::reset()
{
    projects_.clear();
}

void ProjectProjection::apply(const Event& event)
{
    if (event.type != ProjectCreated)
        return;

    ProjectDto p;
    p.id = event.payload["projectId"].toString().toStdString();
    p.name = event.payload["name"].toString().toStdString();
    p.team = event.payload["teamName"].toString().toStdString();
    p.seModel = event.payload["seModel"].toString().toStdString();
    projects_.push_back(p);
}

std::vector<ProjectDto> ProjectProjection::list() const
{
    return projects_;
}