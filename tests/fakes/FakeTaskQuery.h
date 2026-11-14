#pragma once
#include "planova/common/Queries.h"
#include <optional>
#include <utility>
#include <vector>

class FakeTaskQuery : public ITaskQuery {
public:
    void addTask(const TaskDto& t) { tasks_.push_back(t); }
    void setTasks(std::vector<TaskDto> t) { tasks_ = std::move(t); }
    void setAssignee(const Id& taskId, const Id& memberId) {
        for (auto& t : tasks_) if (t.id == taskId) t.assignee = memberId;
    }
    std::vector<TaskDto> all() const override { return tasks_; }
    std::optional<TaskDto> byId(const Id& id) const override {
        for (const auto& t : tasks_) if (t.id == id) return t;
        return std::nullopt;
    }
private:
    std::vector<TaskDto> tasks_;
};
