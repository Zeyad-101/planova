#include "planova/slicer/PizzaPortionCalculator.h"
#include <map>

namespace planova::slicer {

std::vector<Portion> PizzaPortionCalculator::calculate(const std::vector<TaskDto>& tasks,
                                                       const std::vector<MemberDto>& members) {
    std::map<Id, int> counts;
    std::map<Id, int> complexity;
    for (const auto& m : members) {
        counts[m.id] = 0;
        complexity[m.id] = 0;
    }

    int total = 0;
    for (const auto& t : tasks) {
        if (!t.assignee || !complexity.count(*t.assignee)) continue;
        counts[*t.assignee] += 1;
        complexity[*t.assignee] += t.complexity;
        total += t.complexity;
    }

    std::vector<Portion> portions;
    double angle = 0.0;
    for (const auto& m : members) {
        Portion p;
        p.memberId = m.id;
        p.memberName = m.name;
        p.taskCount = counts[m.id];
        p.totalComplexity = complexity[m.id];
        p.startAngle = angle;
        if (total > 0 && p.totalComplexity > 0) {
            p.share = static_cast<double>(p.totalComplexity) / total;
            p.spanAngle = p.share * 360.0;
            angle += p.spanAngle;
        } else {
            p.thinSlice = true;
        }
        portions.push_back(p);
    }
    return portions;
}

} // namespace planova::slicer
