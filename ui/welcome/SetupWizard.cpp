#include "SetupWizard.h"

#include <QButtonGroup>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QStringList>
#include <QStyle>
#include <QVBoxLayout>

namespace {

const QStringList kSteps = {
    "Foundations", "Timeline", "SE Model", "Team Skills", "Components",
    "Constraints", "Slicer Weights", "Git Sync", "Review"
};

const QStringList kModels = {"Agile/Scrum", "Waterfall", "Spiral", "Prototyping", "Hybrid"};

const QStringList kModelDescriptions = {
    "Short iterative sprints, backlog grooming, and burn-down tracking.",
    "Sequential phases: Requirements, Design, Implementation, Verification.",
    "Risk-driven iterative cycles with early prototyping and evaluations.",
    "Rapid iterative proof-of-concept evolutions before delivery.",
    "Combines Agile task execution with formal milestone gates."
};

const int kLastStep = 8;

// Sets the "state" property used by base.qss and refreshes the style
void setState(QWidget* widget, const char* state)
{
    widget->setProperty("state", state);
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
}

QWidget* makePage(const QString& title, QWidget* parent)
{
    auto* page = new QWidget(parent);
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(14);
    auto* heading = new QLabel(title, page);
    heading->setObjectName("cardTitle");
    layout->addWidget(heading);
    return page;
}

QVBoxLayout* pageLayout(QWidget* page)
{
    return static_cast<QVBoxLayout*>(page->layout());
}

} // namespace

SetupWizard::SetupWizard(PlanovaCore& core, QWidget* parent)
    : QWidget(parent), core_(core), pages_(new QStackedWidget(this))
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 24, 32, 24);
    root->setSpacing(16);

    // Header: title, subtitle, and "Step X of 9" badge
    auto* titles = new QVBoxLayout();
    auto* title = new QLabel("Guided Project Setup", this);
    title->setObjectName("pageTitle");
    auto* subtitle = new QLabel("Configure lifecycle, skill matrix, slicing policy, and repository integration.", this);
    subtitle->setObjectName("pageSubtitle");
    titles->addWidget(title);
    titles->addWidget(subtitle);

    stepBadge_ = new QLabel(this);
    stepBadge_->setObjectName("stepBadge");

    auto* header = new QHBoxLayout();
    header->addLayout(titles);
    header->addStretch();
    header->addWidget(stepBadge_, 0, Qt::AlignTop);
    root->addLayout(header);

    // Step indicator row: numbered circles with names underneath
    auto* indicator = new QHBoxLayout();
    for (int i = 0; i < kSteps.size(); ++i) {
        auto* circle = new QLabel(QString::number(i + 1), this);
        circle->setObjectName("stepCircle");
        circle->setAlignment(Qt::AlignCenter);
        auto* stepName = new QLabel(kSteps[i], this);
        stepName->setObjectName("stepName");
        stepName->setAlignment(Qt::AlignCenter);

        auto* column = new QVBoxLayout();
        column->setSpacing(6);
        column->addWidget(circle, 0, Qt::AlignHCenter);
        column->addWidget(stepName, 0, Qt::AlignHCenter);
        indicator->addLayout(column, 1);

        stepCircles_.append(circle);
        stepNames_.append(stepName);
    }
    root->addLayout(indicator);

    // Card holding the step pages
    auto* card = new QFrame(this);
    card->setObjectName("wizardCard");
    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->addWidget(pages_);
    root->addWidget(card, 1);

    // Step 1: Foundations
    QWidget* foundations = makePage("Step 1: Project Foundations", pages_);
    name_ = new QLineEdit(foundations);
    team_ = new QLineEdit(foundations);
    idea_ = new QPlainTextEdit(foundations);
    auto* form1 = new QFormLayout();
    form1->addRow("Project name", name_);
    form1->addRow("Team name", team_);
    form1->addRow("Description", idea_);
    pageLayout(foundations)->addLayout(form1);
    pages_->addWidget(foundations);

    // Step 2: Timeline (tech stack is shown but not saved yet)
    QWidget* timeline = makePage("Step 2: Timeline & Tech Stack", pages_);
    stack_ = new QLineEdit(timeline);
    auto* form2 = new QFormLayout();
    form2->addRow("Tech stack", stack_);
    pageLayout(timeline)->addLayout(form2);
    pages_->addWidget(timeline);

    // Step 3: SE model cards
    QWidget* model = makePage("Step 3: Process Lifecycle Model", pages_);
    modelGroup_ = new QButtonGroup(this);
    auto* cards = new QVBoxLayout();
    cards->setSpacing(10);
    for (int i = 0; i < kModels.size(); ++i) {
        auto* modelCard = new QPushButton(kModels[i] + "\n" + kModelDescriptions[i], model);
        modelCard->setObjectName("modelCard");
        modelCard->setCheckable(true);
        modelCard->setMinimumHeight(64);
        modelGroup_->addButton(modelCard, i);
        cards->addWidget(modelCard);
    }
    modelGroup_->button(0)->setChecked(true);
    pageLayout(model)->addLayout(cards);
    pages_->addWidget(model);

    // Steps 4 to 8: placeholders until the owning slices are built
    const QStringList owners = {"S2", "team decision needed", "team decision needed", "S4", "S2"};
    for (int i = 0; i < owners.size(); ++i) {
        QWidget* page = makePage(QString("Step %1: %2").arg(i + 4).arg(kSteps[i + 3]), pages_);
        auto* note = new QLabel(QString("Coming in a later slice (%1).").arg(owners[i]), page);
        note->setObjectName("placeholder");
        pageLayout(page)->addWidget(note);
        pages_->addWidget(page);
    }

    // Step 9: review
    QWidget* review = makePage("Step 9: Launch & Final Review", pages_);
    summary_ = new QLabel(review);
    summary_->setWordWrap(true);
    pageLayout(review)->addWidget(summary_);
    pages_->addWidget(review);

    // Navigation buttons
    previous_ = new QPushButton("← Previous", this);
    previous_->setObjectName("secondaryButton");
    next_ = new QPushButton("Next Step →", this);
    next_->setObjectName("primaryButton");

    auto* nav = new QHBoxLayout();
    nav->addStretch();
    nav->addWidget(previous_);
    nav->addWidget(next_);
    root->addLayout(nav);

    connect(previous_, &QPushButton::clicked, this, [this] { goToStep(current_ - 1); });
    connect(next_, &QPushButton::clicked, this, [this] {
        if (current_ == kLastStep)
            finish();
        else
            goToStep(current_ + 1);
    });
    connect(name_, &QLineEdit::textChanged, this, [this] { updateNavigation(); });
    connect(team_, &QLineEdit::textChanged, this, [this] { updateNavigation(); });

    goToStep(0);
}

void SetupWizard::reset()
{
    name_->clear();
    team_->clear();
    idea_->clear();
    stack_->clear();
    modelGroup_->button(0)->setChecked(true);
    goToStep(0);
}

void SetupWizard::goToStep(int step)
{
    current_ = step;
    pages_->setCurrentIndex(step);

    for (int i = 0; i < stepCircles_.size(); ++i) {
        const char* state = i < step ? "done" : (i == step ? "active" : "todo");
        setState(stepCircles_[i], state);
        setState(stepNames_[i], state);
        stepCircles_[i]->setText(i < step ? QString::fromUtf8("\u2713") : QString::number(i + 1));
    }

    stepBadge_->setText(QString("Step %1 of 9").arg(step + 1));
    if (step == kLastStep)
        updateReview();
    updateNavigation();
}

void SetupWizard::updateNavigation()
{
    const bool foundationsFilled = !name_->text().trimmed().isEmpty()
    && !team_->text().trimmed().isEmpty();

    previous_->setEnabled(current_ > 0);
    next_->setEnabled(current_ != 0 || foundationsFilled);
    next_->setText(current_ == kLastStep ? "Generate & Launch Workspace  ✓" : "Next Step  →");
}

void SetupWizard::updateReview()
{
    summary_->setText(QString("Project <b>%1</b> is configured with <b>%2</b> methodology for team <b>%3</b>.")
                          .arg(name_->text().trimmed(),
                               kModels.value(modelGroup_->checkedId(), kModels.first()),
                               team_->text().trimmed()));
}

void SetupWizard::finish()
{
    const QString model = kModels.value(modelGroup_->checkedId(), kModels.first());
    core_.projects().create(
        name_->text().trimmed().toStdString(),
        team_->text().trimmed().toStdString(),
        model.toStdString(),
        idea_->toPlainText().toStdString());
    emit finished();
}