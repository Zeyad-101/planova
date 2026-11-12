#include "MainWindow.h"
#include "welcome/WelcomePage.h"
#include "welcome/ProjectsHubPage.h"
#include "welcome/SetupWizard.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QStringList>
#include <QVBoxLayout>

namespace {
const QStringList kScreens = {
    "Welcome", "Projects Hub", "Chaos Slicer (Pizza)", "Pins & Collab",
    "Team & Skills", "Phase Board", "Fair Tracking & Charts", "GitHub Integration", "Project Home"
};
}

MainWindow::MainWindow(PlanovaCore& core, CoreBridge& bridge, QWidget* parent)
    : QMainWindow(parent), core_(core), bridge_(bridge), pages_(new QStackedWidget(this))
{
    setWindowTitle("Planova");
    resize(1200, 760);

    auto* central = new QWidget(this);
    auto* layout = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(buildSidebar());
    layout->addWidget(pages_, 1);
    setCentralWidget(central);

    buildPages();

    connect(&bridge_, &CoreBridge::changed, this, [this](const QString& eventType) {
        statusBar()->showMessage("Last change: " + eventType, 3000);
    });
    statusBar()->showMessage("Workspace: Ready  |  Core Engine: C++17");
}

void MainWindow::buildPages()
{
    auto* welcome = new WelcomePage(pages_);
    auto* hub = new ProjectsHubPage(core_, bridge_, pages_);
    pages_->addWidget(welcome);
    pages_->addWidget(hub);

    connect(welcome, &WelcomePage::continueProjects, this, [this] {
        pages_->setCurrentIndex(1);
    });

    connect(welcome, &WelcomePage::startNewProject, this, [this] {
        SetupWizard wizard(core_, this);
        wizard.exec();
    });

    for (int i = 2; i < kScreens.size(); ++i) {
        auto* page = new QLabel(kScreens[i] + " - Coming soon", pages_);
        page->setAlignment(Qt::AlignCenter);
        pages_->addWidget(page);
    }
}

QWidget* MainWindow::buildSidebar()
{
    auto* sidebar = new QWidget(this);
    sidebar->setFixedWidth(220);
    auto* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(12, 16, 12, 16);

    for (int i = 0; i < kScreens.size(); ++i) {
        auto* button = new QPushButton(kScreens[i], sidebar);
        connect(button, &QPushButton::clicked, this, [this, i] {
            pages_->setCurrentIndex(i);
        });
        layout->addWidget(button);
    }
    layout->addStretch();
    return sidebar;
}