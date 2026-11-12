#include "planova/PlanovaCore.h"

#include <QDateTime>
#include <utility>

int64_t SystemClock::nowMs() const
{
    return QDateTime::currentMSecsSinceEpoch();
}

PlanovaCore::PlanovaCore(std::filesystem::path workspaceDir)
    : workspaceDir_(std::move(workspaceDir)),
    store_(workspaceDir_),
    log_(store_),
    clock_(),
    projects_(log_, clock_)
{
    log_.rebuild();
}

void PlanovaCore::openProject(const Id& projectId)
{
    store_.setDirectory(workspaceDir_ / projectId / "events");
    log_.rebuild();
}

ProjectService& PlanovaCore::projects()
{
    return projects_;
}

ChangeBus& PlanovaCore::bus()
{
    return log_.bus();
}