#include "planova/slicer/ScoringWeights.h"
#include <QTest>

using namespace planova::slicer;

class tst_ScoringWeights : public QObject {
    Q_OBJECT
private slots:
    void defaultValuesAreValid() {
        ScoringWeights w;
        QCOMPARE(w.skill, 50);
        QCOMPARE(w.workload, 30);
        QCOMPARE(w.priority, 20);
        QVERIFY(w.valid());
    }
    void sumMustBe100() {
        QVERIFY(!(ScoringWeights{50, 30, 30}.valid()));
        QVERIFY(!(ScoringWeights{40, 30, 20}.valid()));
        QVERIFY((ScoringWeights{0, 100, 0}.valid()));
    }
    void negativeWeightsInvalid() {
        QVERIFY(!(ScoringWeights{110, -10, 0}.valid()));
    }
    void equalityOperators() {
        QVERIFY((ScoringWeights{50, 30, 20}) == (ScoringWeights{50, 30, 20}));
        QVERIFY((ScoringWeights{50, 30, 20}) != (ScoringWeights{40, 40, 20}));
    }
};

QTEST_APPLESS_MAIN(tst_ScoringWeights)
#include "tst_ScoringWeights.moc"
