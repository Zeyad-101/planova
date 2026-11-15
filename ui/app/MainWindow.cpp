#include "MainWindow.h"
#include "welcome/WelcomePage.h"
#include "welcome/ProjectsHubPage.h"
#include "welcome/SetupWizard.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QStatusBar>
#include <QStringList>
#include <QVBoxLayout>

namespace {

// Index of each page in the QStackedWidget
enum Page { kWelcome, kHub, kWizard, kProjectHome, kSlicer, kPins, kTeam, kPhase, kTracking, kGitHub, kPageCount };

const QStringList kPageNames = {
    "Welcome", "Projects Hub", "Setup Wizard", "Project Home", "Chaos Slicer (Pizza)",
    "Pins & Collab", "Team & Skills", "Phase Board", "Fair Tracking & Charts", "GitHub Integration"
};

} // namespace

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

    statusBar()->setObjectName("appStatusBar");
    connect(&bridge_, &CoreBridge::changed, this, [this](const QString& eventType) {
        statusBar()->showMessage("Last change: " + eventType, 3000);
    });
    statusBar()->showMessage("Workspace: Ready  |  Core Engine: C++17");
}

void MainWindow::buildPages()
{
    auto* welcome = new WelcomePage(pages_);
    auto* hub = new ProjectsHubPage(core_, bridge_, pages_);
    wizard_ = new SetupWizard(core_, pages_);
    pages_->addWidget(welcome);
    pages_->addWidget(hub);
    pages_->addWidget(wizard_);

    connect(welcome, &WelcomePage::continueProjects, this, [this] {
        pages_->setCurrentIndex(kHub);
    });
    connect(welcome, &WelcomePage::startNewProject, this, [this] {
        wizard_->reset();
        pages_->setCurrentIndex(kWizard);
    });
    connect(wizard_, &SetupWizard::finished, this, [this] {
        pages_->setCurrentIndex(kHub);
    });

    for (int i = kProjectHome; i < kPageCount; ++i) {
        auto* page = new QLabel(kPageNames[i] + " - Coming soon", pages_);
        page->setObjectName("placeholder");
        page->setAlignment(Qt::AlignCenter);
        pages_->addWidget(page);
    }
}

QWidget* MainWindow::buildSidebar()
{
    auto* sidebar = new QFrame(this);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(230);
    auto* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(14, 20, 14, 16);
    layout->setSpacing(4);

    // Each page index maps to its sidebar button, so the highlight follows the page
    QMap<int, QPushButton*> navButtons;

    auto addSection = [&](const QString& text) {
        auto* label = new QLabel(text, sidebar);
        label->setObjectName("navSection");
        layout->addWidget(label);
    };

    auto addNav = [&](const QString& text, int page) -> QPushButton* {
        auto* button = new QPushButton(text, sidebar);
        button->setObjectName("navButton");
        button->setCheckable(true);
        connect(button, &QPushButton::clicked, this, [this, page] {
            pages_->setCurrentIndex(page);
        });
        navButtons.insert(page, button);
        layout->addWidget(button);
        return button;
    };

    addSection("MAIN NAVIGATION");
    addNav("Project Home", kProjectHome);
    QPushButton* hubButton = addNav("Projects Hub", kWelcome);
    navButtons.insert(kHub, hubButton);
    addNav("Chaos Slicer (Pizza)", kSlicer);

    addSection("TEAM & COLLABORATION");
    addNav("Pins & Collab", kPins);
    addNav("Team & Skills", kTeam);
    addNav("Phase Board", kPhase);
    addNav("Fair Tracking & Charts", kTracking);
    addNav("GitHub Integration", kGitHub);

    // Highlight the button for the current page
    connect(pages_, &QStackedWidget::currentChanged, this, [navButtons](int index) {
        for (auto* button : navButtons)
            button->setChecked(false);
        if (navButtons.contains(index))
            navButtons.value(index)->setChecked(true);
    });
    hubButton->setChecked(true);

    layout->addStretch();

    // Sync card at the bottom
    auto* syncCard = new QFrame(sidebar);
    syncCard->setObjectName("syncCard");
    auto* syncLayout = new QHBoxLayout(syncCard);
    auto* syncText = new QVBoxLayout();
    auto* syncTitle = new QLabel("Synced with Git", syncCard);
    syncTitle->setObjectName("syncTitle");
    auto* syncSub = new QLabel("Peer event store", syncCard);
    syncSub->setObjectName("syncSub");
    syncText->addWidget(syncTitle);
    syncText->addWidget(syncSub);
    auto* refresh = new QPushButton("\u21BB", syncCard);
    refresh->setObjectName("syncButton");
    syncLayout->addLayout(syncText);
    syncLayout->addStretch();
    syncLayout->addWidget(refresh);
    layout->addWidget(syncCard);

    return sidebar;
}