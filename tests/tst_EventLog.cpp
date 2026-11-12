#include <QtTest>
#include <QJsonObject>
#include "planova/store/EventLog.h"

namespace {

class MemoryStore : public IEventStore {
public:
    std::vector<Event> events;
    void append(const Event& e) override { events.push_back(e); }
    std::vector<Event> readAll() const override { return events; }
};

class CountingProjection : public IProjection {
public:
    int applied = 0;
    void reset() override { applied = 0; }
    void apply(const Event& e) override {
        if (e.type == "ProjectCreated")
            ++applied;
    }
};

Event makeEvent(const std::string& id, int64_t ts, const std::string& type)
{
    Event e;
    e.id = id;
    e.ts = ts;
    e.author = "tester";
    e.type = type;
    e.payload = QJsonObject{};
    return e;
}

} // namespace

class tst_EventLog : public QObject
{
    Q_OBJECT

private slots:
    void rebuildTwiceGivesSameCounts();
    void unknownTypesAreIgnored();
    void appendMatchesRebuild();
    void changeBusFiresOncePerAppend();
};

void tst_EventLog::rebuildTwiceGivesSameCounts()
{
    MemoryStore store;
    store.append(makeEvent("1000-aaaaaaaa", 1000, "ProjectCreated"));
    store.append(makeEvent("2000-bbbbbbbb", 2000, "ProjectCreated"));

    EventLog log(store);
    CountingProjection p;
    log.add(p);

    log.rebuild();
    const int first = p.applied;
    log.rebuild();

    QCOMPARE(first, 2);
    QCOMPARE(p.applied, first);
}

void tst_EventLog::unknownTypesAreIgnored()
{
    MemoryStore store;
    store.append(makeEvent("1000-aaaaaaaa", 1000, "SomethingElse"));

    EventLog log(store);
    CountingProjection p;
    log.add(p);
    log.rebuild();

    QCOMPARE(p.applied, 0);
}

void tst_EventLog::appendMatchesRebuild()
{
    MemoryStore store;
    EventLog log(store);
    CountingProjection p;
    log.add(p);

    log.append(makeEvent("1000-aaaaaaaa", 1000, "ProjectCreated"));
    log.append(makeEvent("2000-bbbbbbbb", 2000, "ProjectCreated"));
    const int live = p.applied;

    log.rebuild();
    QCOMPARE(p.applied, live);
}

void tst_EventLog::changeBusFiresOncePerAppend()
{
    MemoryStore store;
    EventLog log(store);

    int calls = 0;
    log.bus().subscribe([&calls](const std::string&) { ++calls; });

    log.append(makeEvent("1000-aaaaaaaa", 1000, "ProjectCreated"));

    QCOMPARE(calls, 1);
}

QTEST_MAIN(tst_EventLog)
#include "tst_EventLog.moc"