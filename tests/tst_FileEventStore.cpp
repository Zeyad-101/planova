#include <QtTest>
#include <QJsonObject>
#include <QTemporaryDir>
#include "planova/store/FileEventStore.h"

class tst_FileEventStore : public QObject
{
    Q_OBJECT

private slots:
    void appendThenReadAllReturnsSameEvents();
    void readAllSortsByTimeThenId();
    void sameFilesGiveSameList();
};

static Event makeEvent(const std::string& id, int64_t ts)
{
    Event e;
    e.id = id;
    e.ts = ts;
    e.author = "tester";
    e.type = "ProjectCreated";
    e.payload = QJsonObject{{"name", "Planova"}};
    return e;
}

void tst_FileEventStore::appendThenReadAllReturnsSameEvents()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    FileEventStore store(std::filesystem::path(dir.path().toStdString()));

    store.append(makeEvent("1000-aaaaaaaa", 1000));

    const auto events = store.readAll();
    QCOMPARE(events.size(), size_t(1));
    QCOMPARE(QString::fromStdString(events[0].id), QString("1000-aaaaaaaa"));
}

void tst_FileEventStore::readAllSortsByTimeThenId()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    FileEventStore store(std::filesystem::path(dir.path().toStdString()));

    store.append(makeEvent("2000-bbbbbbbb", 2000));
    store.append(makeEvent("1000-aaaaaaaa", 1000));

    const auto events = store.readAll();
    QCOMPARE(events.size(), size_t(2));
    QCOMPARE(QString::fromStdString(events[0].id), QString("1000-aaaaaaaa"));
    QCOMPARE(QString::fromStdString(events[1].id), QString("2000-bbbbbbbb"));
}

void tst_FileEventStore::sameFilesGiveSameList()
{
    QTemporaryDir dirA;
    QTemporaryDir dirB;
    QVERIFY(dirA.isValid());
    QVERIFY(dirB.isValid());
    FileEventStore storeA(std::filesystem::path(dirA.path().toStdString()));
    FileEventStore storeB(std::filesystem::path(dirB.path().toStdString()));

    storeA.append(makeEvent("1000-aaaaaaaa", 1000));
    storeA.append(makeEvent("2000-bbbbbbbb", 2000));
    storeB.append(makeEvent("2000-bbbbbbbb", 2000));
    storeB.append(makeEvent("1000-aaaaaaaa", 1000));

    const auto a = storeA.readAll();
    const auto b = storeB.readAll();
    QCOMPARE(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
        QCOMPARE(QString::fromStdString(a[i].id), QString::fromStdString(b[i].id));
}

QTEST_MAIN(tst_FileEventStore)
#include "tst_FileEventStore.moc"