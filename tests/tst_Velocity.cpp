#include <QtTest>
#include <QJsonObject>
#include <cmath>
#include <vector>

#include "planova/common/Clock.h"
#include "planova/store/EventLog.h"
#include "planova/team/MemberProjection.h"
#include "planova/team/MemberService.h"
#include "planova/team/VelocityCalculator.h"

using namespace std;

namespace {

class MemoryStore : public IEventStore {
public:
    vector<Event> events;
    void append(const Event& e) override { events.push_back(e); }
    vector<Event> readAll() const override { return events; }
};

class FakeClock : public IClock {
public:
    explicit FakeClock(int64_t startMs = 1000) : now_(startMs) {}
    int64_t nowMs() const override { return now_; }
    void advance(int64_t deltaMs) { now_ += deltaMs; }
private:
    int64_t now_;
};

} // namespace

class tst_Velocity : public QObject
{
    Q_OBJECT

private slots:
    void emaBlendingFormula()
    {
        // Current velocity = 1.0, Estimated = 4.0h, Actual = 2.0h (ratio = 2.0)
        // Blended = 0.7 * 1.0 + 0.3 * 2.0 = 1.3
        double v = VelocityCalculator::update(1.0, 4.0, 2.0);
        QVERIFY(std::abs(v - 1.3) < 1e-6);
    }

    void clampToBounds()
    {
        // Finishing a 100h task in 1h -> ratio = 100.0 -> must clamp to 2.0
        double fast = VelocityCalculator::update(1.8, 100.0, 1.0);
        QCOMPARE(fast, 2.0);

        // Taking 100h on a 1h task -> ratio = 0.01 -> must clamp to 0.5
        double slow = VelocityCalculator::update(0.6, 1.0, 100.0);
        QCOMPARE(slow, 0.5);
    }

    void zeroOrNegativeHoursIgnored()
    {
        QCOMPARE(VelocityCalculator::update(1.2, 0.0, 5.0), 1.2);
        QCOMPARE(VelocityCalculator::update(1.2, 5.0, 0.0), 1.2);
        QCOMPARE(VelocityCalculator::update(1.2, -3.0, 2.0), 1.2);
    }

    void taskMovedUpdatesMemberVelocity()
    {
        MemoryStore store;
        EventLog log(store);
        FakeClock clock(1000);

        MemberProjection proj;
        log.add(proj);

        MemberService svc(log, clock);
        Id memberId = svc.add("Adam", "Backend Lead", true);

        // Initial velocity starts at 1.0
        QCOMPARE(proj.byId(memberId)->velocity, 1.0);

        // 1. Move task_1 to InProgress at t = 1000 ms
        Event startEvt;
        startEvt.id = "evt-start";
        startEvt.ts = clock.nowMs();
        startEvt.author = memberId;
        startEvt.type = "TaskMoved";
        QJsonObject startPayload;
        startPayload["taskId"] = "task_1";
        startPayload["toStatus"] = "InProgress";
        startEvt.payload = startPayload;
        log.append(startEvt);

        // 2. Advance clock by 2 hours (2 * 3,600,000 ms = 7,200,000 ms)
        clock.advance(2 * 3600000LL);

        // 3. Move task_1 (estimated 4.0 hours) to Done -> finished in 2.0 hours!
        Event doneEvt;
        doneEvt.id = "evt-done";
        doneEvt.ts = clock.nowMs();
        doneEvt.author = memberId;
        doneEvt.type = "TaskMoved";
        QJsonObject donePayload;
        donePayload["taskId"] = "task_1";
        donePayload["toStatus"] = "Done";
        donePayload["assigneeId"] = QString::fromStdString(memberId);
        donePayload["estimatedHours"] = 4.0;
        doneEvt.payload = donePayload;
        log.append(doneEvt);

        // Expected velocity: 0.7 * 1.0 + 0.3 * (4.0 / 2.0) = 1.3
        double updatedVel = proj.byId(memberId)->velocity;
        QVERIFY(std::abs(updatedVel - 1.3) < 1e-6);
    }
};

QTEST_MAIN(tst_Velocity)
#include "tst_Velocity.moc"