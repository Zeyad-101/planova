#include "planova/slicer/SlicerService.h"
#include "planova/common/Ids.h"
#include "planova/slicer/PizzaPortionCalculator.h"
#include "planova/slicer/SlicerNames.h"
#include <QJsonObject>
#include <QString>
#include <stdexcept>

namespace planova::slicer {

namespace {
void requireValid(const ScoringWeights& w) {
    if (!w.valid()) throw std::invalid_argument("Scoring weights must be non-negative and sum to 100");
}
} // namespace

SlicerService::SlicerService(ITaskQuery& tasks, IMemberQuery& members, IAssignmentStrategy& strategy,
                             EventLog& log, IClock& clock)
    : tasks_(tasks), members_(members), strategy_(strategy), log_(log), clock_(clock) {}

std::vector<Assignment> SlicerService::preview(const ScoringWeights& weights) const {
    requireValid(weights);
    return strategy_.assign(tasks_.all(), members_.all(), weights);
}

void SlicerService::apply(const std::vector<Assignment>& assignments, const ScoringWeights& weights) {
    requireValid(weights);
    const int64_t now = clock_.nowMs();
    bool anyChanged = false;

    for (const auto& a : assignments) {
        auto task = tasks_.byId(a.taskId);
        if (!task) continue;
        if (task->assignee && *task->assignee == a.memberId) continue;  // unchanged

        QJsonObject payload;
        payload["taskId"] = QString::fromStdString(a.taskId);
        payload["memberId"] = QString::fromStdString(a.memberId);
        payload["by"] = QString::fromLatin1(SLICER_AUTHOR);

        Event e;
        e.ts = now;
        e.author = SLICER_AUTHOR;
        e.type = TaskAssigned;
        e.payload = payload;
        e.id = makeEventId(e.ts, e.author, e.payload);
        log_.append(e);
        anyChanged = true;
    }

    if (anyChanged || !lastWeights_ || *lastWeights_ != weights) {
        QJsonObject payload;
        payload["skill"] = weights.skill;
        payload["workload"] = weights.workload;
        payload["priority"] = weights.priority;

        Event e;
        e.ts = now;
        e.author = SLICER_AUTHOR;
        e.type = SlicerWeightsChanged;
        e.payload = payload;
        e.id = makeEventId(e.ts, e.author, e.payload);
        log_.append(e);
        lastWeights_ = weights;
    }
}

std::vector<Portion> SlicerService::portions() const {
    return PizzaPortionCalculator::calculate(tasks_.all(), members_.all());
}

std::vector<Portion> SlicerService::portions(const std::vector<Assignment>& preview) const {
    auto tasks = tasks_.all();
    for (const auto& a : preview)
        for (auto& t : tasks)
            if (t.id == a.taskId && t.status == TaskStatus::ToDo) t.assignee = a.memberId;
    return PizzaPortionCalculator::calculate(tasks, members_.all());
}

} // namespace planova::slicer
