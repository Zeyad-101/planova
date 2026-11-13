#include "slicer/AssignmentTableModel.h"
#include "theme/ThemeManager.h"
#include <QTest>

using namespace planova::slicer;

namespace {
std::vector<MemberDto> members() {
    return {MemberDto{"1", "Alice", "Dev", false, {}, 100, 1.0}, MemberDto{"2", "Bob", "Dev", false, {}, 100, 1.0}};
}
TaskDto task(bool blocked) {
    return TaskDto{"t1", "Auth API", SkillCategory::Backend, Priority::High, 3, 4, TaskStatus::ToDo,
                   std::nullopt, blocked ? std::vector<Id>{"dep"} : std::vector<Id>{}};
}
}

class tst_AssignmentTableModel : public QObject {
    Q_OBJECT
private slots:
    void countsAndText() {
        AssignmentTableModel model;
        model.setAssignments({Assignment{"t1", "1", 85, "Rationale text", false}}, {task(false)}, members());
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.columnCount(), 7);
        QCOMPARE(model.data(model.index(0, 0)).toString(), QString("Auth API (3 pts)"));
        QCOMPARE(model.data(model.index(0, 1)).toString(), QString("Backend"));
        QCOMPARE(model.data(model.index(0, 2)).toString(), QString("High"));
        QCOMPARE(model.data(model.index(0, 4)).toString(), QString("Rationale text"));
        QCOMPARE(model.data(model.index(0, 5)).toString(), QString("Ready"));
    }
    void waitingStatusUsesThemeTokens() {
        AssignmentTableModel model;
        model.setAssignments({Assignment{"t1", "1", 85, "R", true}}, {task(true)}, members());
        QModelIndex idx = model.index(0, 5);
        QCOMPARE(model.data(idx, Qt::DisplayRole).toString(), QString("Waiting on Dep"));
        QCOMPARE(model.data(idx, Qt::BackgroundRole).value<QColor>(),
                 ThemeManager::instance().color(Token::WarningSoft));
        QCOMPARE(model.data(idx, Qt::ForegroundRole).value<QColor>(),
                 ThemeManager::instance().color(Token::TextPrimary));
    }
    void manualOverrideUpdatesAssignment() {
        AssignmentTableModel model;
        model.setAssignments({Assignment{"t1", "1", 85, "Initial", false}}, {task(false)}, members());
        QVERIFY(model.setData(model.index(0, 6), QString("2"), Qt::EditRole));
        auto result = model.currentAssignments();
        QCOMPARE(result[0].memberId, std::string("2"));
        QCOMPARE(result[0].matchPct, 0);
        QCOMPARE(QString::fromStdString(result[0].rationale), QString("Manual override"));
    }
    void unknownMemberIsRejected() {
        AssignmentTableModel model;
        model.setAssignments({Assignment{"t1", "1", 85, "Initial", false}}, {task(false)}, members());
        QVERIFY(!model.setData(model.index(0, 6), QString("99"), Qt::EditRole));
        QCOMPARE(model.currentAssignments()[0].memberId, std::string("1"));
    }
};

QTEST_MAIN(tst_AssignmentTableModel)
#include "tst_AssignmentTableModel.moc"
