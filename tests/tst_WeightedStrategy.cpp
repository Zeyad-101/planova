#include "planova/slicer/WeightedScoringStrategy.h"
#include <QTest>

using namespace planova::slicer;

namespace {
MemberDto member(const char* id, const char* name, std::map<SkillCategory, int> ratings, double velocity = 1.0) {
    return MemberDto{id, name, "Dev", false, std::move(ratings), 100, velocity};
}
TaskDto task(const char* id, Priority p, int complexity, int hours, TaskStatus s = TaskStatus::ToDo,
             std::optional<Id> assignee = std::nullopt, std::vector<Id> deps = {}) {
    return TaskDto{id, id, SkillCategory::Backend, p, complexity, hours, s, std::move(assignee), std::move(deps)};
}
}

class tst_WeightedStrategy : public QObject {
    Q_OBJECT
private slots:
    void skillDominanceWhenWorkloadZero() {
        WeightedScoringStrategy strat;
        auto res = strat.assign({task("t1", Priority::Medium, 3, 5)},
                                {member("1", "Alice", {{SkillCategory::Backend, 5}}),
                                 member("2", "Bob", {{SkillCategory::Backend, 2}})},
                                ScoringWeights{100, 0, 0});
        QCOMPARE(int(res.size()), 1);
        QCOMPARE(res[0].memberId, std::string("1"));
        QCOMPARE(res[0].matchPct, 100);
        QCOMPARE(QString::fromStdString(res[0].rationale),
                 QString("High Backend proficiency (5/5) with balanced load (0h)."));
    }

    void workloadLevelingSplitsEqualTasks() {
        WeightedScoringStrategy strat;
        auto res = strat.assign({task("t1", Priority::Medium, 3, 4), task("t2", Priority::Medium, 3, 4)},
                                {member("1", "Alice", {{SkillCategory::Backend, 5}}),
                                 member("2", "Bob", {{SkillCategory::Backend, 5}})},
                                ScoringWeights{0, 100, 0});
        QCOMPARE(int(res.size()), 2);
        QCOMPARE(res[0].memberId, std::string("1"));  // tie -> Alice
        QCOMPARE(res[1].memberId, std::string("2"));  // Alice now loaded -> Bob
    }

    void startingLoadFromInProgressAndReview() {
        WeightedScoringStrategy strat;
        auto res = strat.assign({task("e1", Priority::Medium, 3, 8, TaskStatus::InProgress, Id("1")),
                                 task("e2", Priority::Medium, 3, 4, TaskStatus::Review, Id("1")),
                                 task("t1", Priority::Medium, 3, 2)},
                                {member("1", "Alice", {{SkillCategory::Backend, 5}}),
                                 member("2", "Bob", {{SkillCategory::Backend, 5}})},
                                ScoringWeights{50, 50, 0});
        QCOMPARE(int(res.size()), 1);
        QCOMPARE(res[0].memberId, std::string("2"));  // Alice starts with 12h
    }

    void eligibilityRatedOnlyWhenSomeRated() {
        // Weights {0,100,0}: without the rule both score equal and Alice (unrated) would win by name.
        WeightedScoringStrategy strat;
        auto res = strat.assign({task("t1", Priority::Medium, 3, 5)},
                                {member("1", "Alice", {}), member("2", "Bob", {{SkillCategory::Backend, 1}})},
                                ScoringWeights{0, 100, 0});
        QCOMPARE(res[0].memberId, std::string("2"));
    }

    void ratedMemberWinsEvenWhenLoaded() {
        // Without the rule, loaded Bob would lose to idle Alice.
        WeightedScoringStrategy strat;
        auto res = strat.assign({task("e1", Priority::Medium, 3, 50, TaskStatus::InProgress, Id("2")),
                                 task("t1", Priority::Medium, 3, 5)},
                                {member("1", "Alice", {}), member("2", "Bob", {{SkillCategory::Backend, 1}})},
                                ScoringWeights{10, 80, 10});
        QCOMPARE(res[0].memberId, std::string("2"));
    }

    void allUnratedFallback() {
        WeightedScoringStrategy strat;
        auto res = strat.assign({task("t1", Priority::Medium, 3, 5)},
                                {member("1", "Alice", {}), member("2", "Bob", {})},
                                ScoringWeights{50, 30, 20});
        QCOMPARE(res[0].memberId, std::string("1"));
        QCOMPARE(QString::fromStdString(res[0].rationale),
                 QString("No rated member for Backend; assigned by workload."));
    }

    void pnScalingForHighVsLow() {
        // The fast member is alphabetically LAST, so a broken Pn cannot pass by name tie-break.
        WeightedScoringStrategy strat;
        std::vector<MemberDto> ms = {member("1", "Adam", {{SkillCategory::Backend, 3}}, 1.0),
                                     member("2", "Zoe", {{SkillCategory::Backend, 3}}, 5.0)};
        auto high = strat.assign({task("tH", Priority::High, 3, 2)}, ms, ScoringWeights{0, 0, 100});
        QCOMPARE(high[0].memberId, std::string("2"));  // velocity decides
        auto low = strat.assign({task("tL", Priority::Low, 3, 2)}, ms, ScoringWeights{0, 0, 100});
        QCOMPARE(low[0].memberId, std::string("1"));   // Pn = 1 for both -> name decides
    }

    void tieBreakByName() {
        WeightedScoringStrategy strat;
        auto res = strat.assign({task("t1", Priority::Medium, 3, 2)},
                                {member("2", "Zoe", {{SkillCategory::Backend, 4}}),
                                 member("1", "Adam", {{SkillCategory::Backend, 4}})},
                                ScoringWeights{50, 30, 20});
        QCOMPARE(res[0].memberId, std::string("1"));
    }

    void nonToDoIgnored() {
        WeightedScoringStrategy strat;
        auto res = strat.assign({task("tD", Priority::High, 3, 2, TaskStatus::Done, Id("1"))},
                                {member("1", "Alice", {{SkillCategory::Backend, 5}})}, ScoringWeights{});
        QCOMPARE(int(res.size()), 0);
    }

    void waitingOnDepFlag() {
        WeightedScoringStrategy strat;
        auto res = strat.assign({task("dep", Priority::Medium, 1, 2, TaskStatus::InProgress, Id("1")),
                                 task("t1", Priority::Medium, 3, 5, TaskStatus::ToDo, std::nullopt, {"dep"})},
                                {member("1", "Alice", {{SkillCategory::Backend, 5}})}, ScoringWeights{});
        QCOMPARE(int(res.size()), 1);
        QVERIFY(res[0].waitingOnDep);
    }

    void determinism() {
        WeightedScoringStrategy strat;
        std::vector<MemberDto> ms = {member("1", "Alice", {{SkillCategory::Backend, 4}}, 2.0),
                                     member("2", "Bob", {{SkillCategory::Backend, 3}}, 3.0)};
        std::vector<TaskDto> ts = {task("t1", Priority::High, 3, 4), task("t2", Priority::High, 3, 4)};
        auto a = strat.assign(ts, ms, ScoringWeights{});
        auto b = strat.assign(ts, ms, ScoringWeights{});
        QCOMPARE(int(a.size()), int(b.size()));
        for (size_t i = 0; i < a.size(); ++i) {
            QCOMPARE(a[i].taskId, b[i].taskId);
            QCOMPARE(a[i].memberId, b[i].memberId);
            QCOMPARE(a[i].matchPct, b[i].matchPct);
        }
    }

    void matchPctBounds() {
        WeightedScoringStrategy strat;
        // Perfect: rating 5/5, no load, velocity 5.0 -> score 1.0 -> 100
        auto top = strat.assign({task("t1", Priority::High, 3, 4)},
                                {member("1", "Alice", {{SkillCategory::Backend, 5}}, 5.0)}, ScoringWeights{});
        QCOMPARE(top[0].matchPct, 100);
        // Worst: unrated, loaded (L = 0), velocity 0 -> score 0 -> 0
        auto worst = strat.assign({task("e1", Priority::Medium, 5, 100, TaskStatus::InProgress, Id("2")),
                                   task("t1", Priority::High, 3, 4)},
                                  {member("2", "Bob", {}, 0.0)}, ScoringWeights{});
        QCOMPARE(worst[0].matchPct, 0);
    }
};

QTEST_APPLESS_MAIN(tst_WeightedStrategy)
#include "tst_WeightedStrategy.moc"
