#pragma once
#include "planova/common/Clock.h"
#include "planova/common/Dtos.h"
#include "planova/common/Types.h"
#include "planova/project/ProjectProjection.h"
#include "planova/store/EventLog.h"
#include <string>
#include <vector>

class ProjectService {
public:
    ProjectService(EventLog& log, IClock& clock);
    Id create(const std::string& name, const std::string& team,
              const std::string& seModel, const std::string& idea);
    std::vector<ProjectDto> list() const;

private:
    EventLog& log_;
    IClock& clock_;
    ProjectProjection projection_;
};