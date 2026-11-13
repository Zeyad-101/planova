#include "slicer/ChaosSlicerPage.h"
#include "theme/ThemeManager.h"
#include "planova/store/EventLog.h"
#include "planova/common/IProjection.h"
#include "planova/slicer/WeightedScoringStrategy.h"
#include "planova/slicer/SlicerService.h"
#include "planova/slicer/SlicerNames.h"
#include "tests/fakes/FakeClock.h"
#include "tests/fakes/FakeEventStore.h"
#include "tests/fakes/FakeMemberQuery.h"
#include "tests/fakes/FakeTaskQuery.h"
#include <QApplication>
#include <QMainWindow>
#include <QPixmap>
#include <QScrollArea>
#include <QScrollBar>
#include <QStyleFactory>
#include <QTimer>

class AssignProjection : public IProjection {
public:
    explicit AssignProjection(FakeTaskQuery& tq) : tq_(tq) {}
    void reset() override {}
    void apply(const Event& e) override {
        if (e.type == planova::slicer::TaskAssigned) {
            tq_.setAssignee(e.payload["taskId"].toString().toStdString(),
                            e.payload["memberId"].toString().toStdString());
        }
    }
private:
    FakeTaskQuery& tq_;
};

int main(int argc, char* argv[]) {
    Q_INIT_RESOURCE(planova);
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));

    auto& tm = ThemeManager::instance();
    tm.apply(ThemeMode::Light);

    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window, tm.color(Token::Background));
    lightPalette.setColor(QPalette::WindowText, tm.color(Token::TextPrimary));
    lightPalette.setColor(QPalette::Base, tm.color(Token::Surface));
    lightPalette.setColor(QPalette::AlternateBase, tm.color(Token::SurfaceAlt));
    lightPalette.setColor(QPalette::ToolTipBase, tm.color(Token::Surface));
    lightPalette.setColor(QPalette::ToolTipText, tm.color(Token::TextPrimary));
    lightPalette.setColor(QPalette::Text, tm.color(Token::TextPrimary));
    lightPalette.setColor(QPalette::Button, tm.color(Token::Surface));
    lightPalette.setColor(QPalette::ButtonText, tm.color(Token::TextPrimary));
    lightPalette.setColor(QPalette::BrightText, tm.color(Token::AccentText));
    lightPalette.setColor(QPalette::Link, tm.color(Token::Accent));
    lightPalette.setColor(QPalette::Highlight, tm.color(Token::Accent));
    lightPalette.setColor(QPalette::HighlightedText, tm.color(Token::AccentText));
    app.setPalette(lightPalette);

    FakeTaskQuery tq;
    FakeMemberQuery mq;
    FakeEventStore store;
    FakeClock clock(1000);
    EventLog log(store);

    AssignProjection proj(tq);
    log.add(proj);

    // 6 members with varied skill ratings
    mq.addMember(MemberDto{"m1", "Alice", "Frontend Lead", true,
                           {{SkillCategory::Frontend, 5}, {SkillCategory::UiUx, 4}}, 100, 4.5});
    mq.addMember(MemberDto{"m2", "Bob", "Backend Dev", false,
                           {{SkillCategory::Backend, 5}, {SkillCategory::Integration, 3}}, 90, 3.8});
    mq.addMember(MemberDto{"m3", "Charlie", "Fullstack", false,
                           {{SkillCategory::Frontend, 3}, {SkillCategory::Backend, 4}}, 80, 4.0});
    mq.addMember(MemberDto{"m4", "Dana", "DevOps & Cloud", false,
                           {{SkillCategory::Integration, 5}, {SkillCategory::Storage, 2}}, 85, 3.5});
    mq.addMember(MemberDto{"m5", "Eve", "UI/UX Designer", false,
                           {{SkillCategory::UiUx, 5}, {SkillCategory::Frontend, 2}}, 95, 4.2});
    mq.addMember(MemberDto{"m6", "Frank", "QA & Testing", false,
                           {{SkillCategory::Testing, 5}, {SkillCategory::Backend, 2}}, 75, 3.0});

    // 8 tasks covering ToDo, InProgress, Review, Done, one with blockedBy
    tq.addTask(TaskDto{"t1", "Design Landing Page", SkillCategory::UiUx, Priority::High, 3, 6,
                       TaskStatus::Done, "m5", {}});
    tq.addTask(TaskDto{"t2", "Setup CI/CD Pipeline", SkillCategory::Integration, Priority::Critical, 4, 8,
                       TaskStatus::InProgress, "m4", {}});
    tq.addTask(TaskDto{"t3", "User Auth API", SkillCategory::Backend, Priority::High, 4, 10,
                       TaskStatus::Review, "m2", {}});
    tq.addTask(TaskDto{"t4", "Build Navigation Menu", SkillCategory::Frontend, Priority::Medium, 2, 4,
                       TaskStatus::ToDo, std::nullopt, {}});
    tq.addTask(TaskDto{"t5", "Database Migration", SkillCategory::Storage, Priority::High, 3, 6,
                       TaskStatus::ToDo, std::nullopt, {}});
    tq.addTask(TaskDto{"t6", "End-to-End Test Suite", SkillCategory::Testing, Priority::Low, 2, 5,
                       TaskStatus::ToDo, std::nullopt, {"t4"}});
    tq.addTask(TaskDto{"t7", "Dashboard Analytics UI", SkillCategory::Frontend, Priority::Critical, 5, 12,
                       TaskStatus::ToDo, std::nullopt, {}});
    tq.addTask(TaskDto{"t8", "Payment Gateway Integration", SkillCategory::Backend, Priority::Critical, 5, 14,
                       TaskStatus::ToDo, std::nullopt, {"t3"}});

    planova::slicer::WeightedScoringStrategy strategy;
    planova::slicer::SlicerService slicer(tq, mq, strategy, log, clock);

    QString screenshotPath;
    QString screenshotTablePath;
    for (int i = 1; i < argc; ++i) {
        if (QString(argv[i]) == "--screenshot" && i + 1 < argc) {
            screenshotPath = argv[++i];
        } else if (QString(argv[i]) == "--screenshot-table" && i + 1 < argc) {
            screenshotTablePath = argv[++i];
        }
    }

    QMainWindow window;
    window.setWindowTitle("Planova Chaos Slicer (dev harness)");
    window.resize(1280, 800);

    auto* page = new planova::slicer::ChaosSlicerPage(slicer, tq, mq, &window);
    log.bus().subscribe([page](const std::string&) {
        page->refreshFromBoard();
    });

    window.setCentralWidget(page);
    window.show();

    if (!screenshotPath.isEmpty()) {
        QTimer::singleShot(300, [&window, screenshotPath]() {
            QPixmap pix = window.grab();
            pix.save(screenshotPath);
            QApplication::exit(0);
        });
    } else if (!screenshotTablePath.isEmpty()) {
        QTimer::singleShot(300, [&window, page, screenshotTablePath]() {
            auto* scroll = page->findChild<QScrollArea*>("slicerScrollArea");
            if (scroll && scroll->verticalScrollBar()) {
                scroll->verticalScrollBar()->setValue(scroll->verticalScrollBar()->maximum());
            }
            QTimer::singleShot(150, [&window, screenshotTablePath]() {
                QPixmap pix = window.grab();
                pix.save(screenshotTablePath);
                QApplication::exit(0);
            });
        });
    }

    return app.exec();
}
