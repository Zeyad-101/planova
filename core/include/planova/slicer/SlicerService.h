#pragma once
#include "planova/common/Clock.h"
#include "planova/common/Queries.h"
#include "planova/slicer/IAssignmentStrategy.h"
#include "planova/slicer/ScoringWeights.h"
#include "planova/slicer/SlicerTypes.h"
#include "planova/store/EventLog.h"
#include <optional>
#include <vector>

namespace planova::slicer {

class SlicerService {
public:
    static constexpr const char* SLICER_AUTHOR = "chaos-slicer";

    SlicerService(ITaskQuery& tasks, IMemberQuery& members, IAssignmentStrategy& strategy,
                  EventLog& log, IClock& clock);

    // Throws std::invalid_argument if the weights are invalid. Appends no events.
    std::vector<Assignment> preview(const ScoringWeights& weights) const;
    // Appends TaskAssigned for each changed task, and SlicerWeightsChanged on first apply,
    // changed weights, or any changed assignee. Identical re-apply appends nothing.
    void apply(const std::vector<Assignment>& assignments, const ScoringWeights& weights);
    std::vector<Portion> portions() const;                                          // board state
    std::vector<Portion> portions(const std::vector<Assignment>& preview) const;    // overlay on ToDo tasks

private:
    ITaskQuery& tasks_;
    IMemberQuery& members_;
    IAssignmentStrategy& strategy_;
    EventLog& log_;
    IClock& clock_;
    std::optional<ScoringWeights> lastWeights_ = std::nullopt;
};

} // namespace planova::slicer
