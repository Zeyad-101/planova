#include "slicer/WeightRebalance.h"
#include <QTest>
#include <cstdlib>

using namespace planova::slicer;

class tst_WeightRebalance : public QObject {
    Q_OBJECT
private slots:
    void skillMovedKeepsSum100() {
        auto r = rebalance(ScoringWeights{50, 30, 20}, 0, 70);
        QCOMPARE(r.skill, 70);
        QCOMPARE(r.workload, 18);
        QCOMPARE(r.priority, 12);
        QVERIFY(r.valid());
    }
    void maxValueZeroesOthers() {
        auto r = rebalance(ScoringWeights{50, 30, 20}, 0, 100);
        QCOMPARE(r.skill, 100);
        QCOMPARE(r.workload, 0);
        QCOMPARE(r.priority, 0);
    }
    void bothOthersZeroSplitEvenly() {
        auto r = rebalance(ScoringWeights{100, 0, 0}, 0, 40);
        QCOMPARE(r.workload, 30);
        QCOMPARE(r.priority, 30);
    }
    void oddRemainderStillSums100() {
        auto r = rebalance(ScoringWeights{34, 33, 33}, 0, 41);
        QCOMPARE(r.skill + r.workload + r.priority, 100);
        QVERIFY(r.workload >= 0 && r.priority >= 0);
        QVERIFY(std::abs(r.workload - r.priority) <= 1);
    }
    void priorityMoved() {
        auto r = rebalance(ScoringWeights{50, 30, 20}, 2, 0);
        QCOMPARE(r.priority, 0);
        QCOMPARE(r.skill + r.workload, 100);
        QVERIFY(r.valid());
    }
    void everyPositionAlwaysSums100() {
        for (int index = 0; index < 3; ++index)
            for (int v = 0; v <= 100; ++v)
                QVERIFY(rebalance(ScoringWeights{50, 30, 20}, index, v).valid());
    }
};

QTEST_APPLESS_MAIN(tst_WeightRebalance)
#include "tst_WeightRebalance.moc"
