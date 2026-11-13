#include "planova/slicer/WeightedScoringStrategy.h"
#include "planova/slicer/SlicerNames.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <set>

namespace planova::slicer {

namespace {
constexpr double kEpsilon = 1e-9;

int ratingOf(const MemberDto& m, SkillCategory skill) {
    auto it = m.ratings.find(skill);
    return it == m.ratings.end() ? 0 : it->second;
}
} // namespace

std::vector<Assignment> WeightedScoringStrategy::assign(const std::vector<TaskDto>& tasks,
                                                        const std::vector<MemberDto>& members,
                                                        const ScoringWeights& weights) const {
    std::vector<Assignment> result;
    if (members.empty()) return result;

    // Starting load: InProgress and Review tasks already count against a member.
    std::map<Id, int> hours;
    for (const auto& m : members) hours[m.id] = 0;
    std::set<Id> doneIds;
    for (const auto& t : tasks) {
        if (t.status == TaskStatus::Done) doneIds.insert(t.id);
        bool active = t.status == TaskStatus::InProgress || t.status == TaskStatus::Review;
        if (active && t.assignee && hours.count(*t.assignee)) hours[*t.assignee] += t.estHours;
    }

    std::vector<TaskDto> todo;
    for (const auto& t : tasks)
        if (t.status == TaskStatus::ToDo) todo.push_back(t);

    // Priority desc, complexity desc, id asc (id keeps the order deterministic).
    std::sort(todo.begin(), todo.end(), [](const TaskDto& a, const TaskDto& b) {
        if (a.priority != b.priority) return static_cast<int>(a.priority) > static_cast<int>(b.priority);
        if (a.complexity != b.complexity) return a.complexity > b.complexity;
        return a.id < b.id;
    });

    const double ws = weights.skill / 100.0;
    const double wl = weights.workload / 100.0;
    const double wp = weights.priority / 100.0;

    for (const auto& task : todo) {
        bool waiting = false;
        for (const auto& dep : task.blockedBy)
            if (!doneIds.count(dep)) waiting = true;

        // If anyone is rated for this skill, only rated members are eligible.
        bool anyRated = std::any_of(members.begin(), members.end(),
                                    [&](const MemberDto& m) { return ratingOf(m, task.skill) > 0; });

        int maxHours = 0;
        for (const auto& kv : hours) maxHours = std::max(maxHours, kv.second);
        const double divisor = std::max(1, maxHours);

        const MemberDto* winner = nullptr;
        double best = 0.0;
        for (const auto& m : members) {
            int rating = ratingOf(m, task.skill);
            if (anyRated && rating == 0) continue;

            double s = rating / 5.0;
            double l = 1.0 - hours[m.id] / divisor;
            bool urgent = task.priority == Priority::High || task.priority == Priority::Critical;
            double pn = urgent ? std::min(m.velocity, 5.0) / 5.0 : 1.0;
            double score = ws * s + wl * l + wp * pn;

            if (winner == nullptr || score > best + kEpsilon) {
                winner = &m;
                best = score;
            } else if (std::abs(score - best) <= kEpsilon && m.name < winner->name) {
                winner = &m;  // tie: name ascending
            }
        }
        if (winner == nullptr) continue;

        int rating = ratingOf(*winner, task.skill);
        std::string rationale =
            rating > 0 ? "High " + skillToString(task.skill) + " proficiency (" + std::to_string(rating) +
                             "/5) with balanced load (" + std::to_string(hours[winner->id]) + "h)."
                       : "No rated member for " + skillToString(task.skill) + "; assigned by workload.";
        int matchPct = std::clamp(static_cast<int>(std::round(best * 100.0)), 0, 100);

        result.push_back(Assignment{task.id, winner->id, matchPct, rationale, waiting});
        hours[winner->id] += task.estHours;
    }
    return result;
}

} // namespace planova::slicer
