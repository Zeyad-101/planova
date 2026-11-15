#include <QtTest>
#include <stdexcept>
#include <vector>

#include "planova/common/Clock.h"
#include "planova/store/EventLog.h"
#include "planova/team/MemberProjection.h"
#include "planova/team/MemberService.h"
#include "planova/team/SkillCategory.h"

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

class tst_MemberService : public QObject
{
    Q_OBJECT

private slots:
    void addAndQueryMembers()
    {
        MemoryStore store;
        EventLog log(store);
        FakeClock clock(1000);

        MemberProjection proj;
        log.add(proj);

        MemberService svc(log, clock);

        Id id1 = svc.add("Adam", "Backend Lead", true);
        clock.advance(10);
        Id id2 = svc.add("Zeyad", "UI Designer", false);

        auto allMembers = proj.all();
        QCOMPARE(allMembers.size(), size_t(2));
        QCOMPARE(allMembers[0].id, id1);
        QCOMPARE(allMembers[0].name, string("Adam"));
        QCOMPARE(allMembers[0].role, string("Backend Lead"));
        QCOMPARE(allMembers[0].isLead, true);

        QCOMPARE(allMembers[1].id, id2);
        QCOMPARE(allMembers[1].name, string("Zeyad"));
        QCOMPARE(allMembers[1].isLead, false);

        auto found = proj.byId(id1);
        QVERIFY(found.has_value());
        QCOMPARE(found->name, string("Adam"));

        auto missing = proj.byId("non_existent_id");
        QVERIFY(!missing.has_value());
    }

    void emptyNameThrows()
    {
        MemoryStore store;
        EventLog log(store);
        FakeClock clock(1000);
        MemberService svc(log, clock);

        QVERIFY_THROWS_EXCEPTION(invalid_argument, svc.add("", "Developer", false));
    }

    void updateRoleAndClampUnderstanding()
    {
        MemoryStore store;
        EventLog log(store);
        FakeClock clock(1000);

        MemberProjection proj;
        log.add(proj);

        MemberService svc(log, clock);
        Id id = svc.add("Adam", "Junior Dev", false);

        // Normal update
        clock.advance(10);
        svc.update(id, "Senior Dev", 85);
        QCOMPARE(proj.byId(id)->role, string("Senior Dev"));
        QCOMPARE(proj.byId(id)->understanding, 85);

        // Clamp above 100 -> 100
        clock.advance(10);
        svc.update(id, "Senior Dev", 150);
        QCOMPARE(proj.byId(id)->understanding, 100);

        // Clamp below 0 -> 0
        clock.advance(10);
        svc.update(id, "Senior Dev", -25);
        QCOMPARE(proj.byId(id)->understanding, 0);
    }

    void rateSkillsAndValidateRange()
    {
        MemoryStore store;
        EventLog log(store);
        FakeClock clock(1000);

        MemberProjection proj;
        log.add(proj);

        MemberService svc(log, clock);
        Id id = svc.add("Adam", "Backend", true);

        clock.advance(10);
        svc.rate(id, SkillCategory::Backend, 5);
        clock.advance(10);
        svc.rate(id, SkillCategory::Frontend, 4);

        auto dto = proj.byId(id);
        QVERIFY(dto.has_value());
        QCOMPARE(dto->ratings.at(SkillCategory::Backend), 5);
        QCOMPARE(dto->ratings.at(SkillCategory::Frontend), 4);

        // Out-of-bounds ratings (0 or 6) must throw invalid_argument
        QVERIFY_THROWS_EXCEPTION(invalid_argument, svc.rate(id, SkillCategory::Backend, 0));
        QVERIFY_THROWS_EXCEPTION(invalid_argument, svc.rate(id, SkillCategory::Backend, 6));
    }

    void toggleSettingsAndRebuild()
    {
        MemoryStore store;
        EventLog log(store);
        FakeClock clock(1000);

        MemberProjection proj;
        log.add(proj);

        MemberService svc(log, clock);

        // Default before setting
        QCOMPARE(proj.getSetting("auto_assign", false), false);

        clock.advance(10);
        svc.setSetting("auto_assign", true);
        QCOMPARE(proj.getSetting("auto_assign", false), true);

        // Rebuild from EventLog store and verify state matches
        log.rebuild();
        QCOMPARE(proj.getSetting("auto_assign", false), true);
    }
};

QTEST_MAIN(tst_MemberService)
#include "tst_MemberService.moc"