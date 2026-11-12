#include "SetupWizard.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QWizardPage>

namespace {

class FoundationsPage : public QWizardPage {
public:
    explicit FoundationsPage(QWidget* parent)
        : QWizardPage(parent)
    {
        setTitle("Step 1: Project Foundations");
        auto* name = new QLineEdit(this);
        auto* team = new QLineEdit(this);
        auto* idea = new QPlainTextEdit(this);
        auto* form = new QFormLayout(this);
        form->addRow("Project name", name);
        form->addRow("Team name", team);
        form->addRow("Description", idea);
        registerField("projectName*", name);
        registerField("teamName*", team);
        registerField("idea", idea, "plainText", "textChanged");
    }
};

class TimelinePage : public QWizardPage {
public:
    explicit TimelinePage(QWidget* parent)
        : QWizardPage(parent)
    {
        setTitle("Step 2: Timeline & Tech Stack");
        auto* stack = new QLineEdit(this);
        auto* form = new QFormLayout(this);
        form->addRow("Tech stack", stack);
    }
};

class ModelPage : public QWizardPage {
public:
    explicit ModelPage(QWidget* parent)
        : QWizardPage(parent)
    {
        setTitle("Step 3: Process Lifecycle Model");
        auto* combo = new QComboBox(this);
        combo->addItems({"Agile/Scrum", "Waterfall", "Spiral", "Prototyping", "Hybrid"});
        auto* layout = new QVBoxLayout(this);
        layout->addWidget(combo);
        registerField("seModel", combo, "currentText", "currentTextChanged");
    }
};

class ReviewPage : public QWizardPage {
public:
    using QWizardPage::QWizardPage;

    void initializePage() override
    {
        summary_->setText(
            "Project: " + field("projectName").toString() + "\n"
                                                            "Team: " + field("teamName").toString() + "\n"
                                             "Process model: " + field("seModel").toString());
    }

    void setSummaryLabel(QLabel* label) { summary_ = label; }

private:
    QLabel* summary_ = nullptr;
};

QWizardPage* placeholderPage(const QString& title, const QString& owner, QWizard* parent)
{
    auto* page = new QWizardPage(parent);
    page->setTitle(title);
    auto* layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel("Coming in a later slice (" + owner + ").", page));
    return page;
}

} // namespace

SetupWizard::SetupWizard(PlanovaCore& core, QWidget* parent)
    : QWizard(parent), core_(core)
{
    setWindowTitle("Guided Project Setup");

    addPage(new FoundationsPage(this));
    addPage(new TimelinePage(this));
    addPage(new ModelPage(this));

    addPage(placeholderPage("Step 4: Team Skills", "S2", this));
    addPage(placeholderPage("Step 5: Components", "team decision needed", this));
    addPage(placeholderPage("Step 6: Constraints", "team decision needed", this));
    addPage(placeholderPage("Step 7: Slicer Weights", "S4", this));
    addPage(placeholderPage("Step 8: Git Sync", "S2", this));

    auto* review = new ReviewPage(this);
    review->setTitle("Step 9: Launch & Final Review");
    auto* summary = new QLabel(review);
    review->setSummaryLabel(summary);
    auto* layout = new QVBoxLayout(review);
    layout->addWidget(summary);
    addPage(review);

    setButtonText(QWizard::FinishButton, "Generate & Launch Workspace");
}

void SetupWizard::accept()
{
    core_.projects().create(
        field("projectName").toString().toStdString(),
        field("teamName").toString().toStdString(),
        field("seModel").toString().toStdString(),
        field("idea").toString().toStdString());
    QWizard::accept();
}