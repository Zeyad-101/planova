#include "slicer/ChaosSlicerPage.h"
#include "slicer/AssigneeDelegate.h"
#include "slicer/AssignmentTableModel.h"
#include "slicer/PizzaWidget.h"
#include "slicer/WeightSlidersPanel.h"
#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>
#include <stdexcept>

namespace planova::slicer {

ChaosSlicerPage::ChaosSlicerPage(planova::slicer::SlicerService& slicer,
                                 ITaskQuery& tasks,
                                 IMemberQuery& members,
                                 QWidget* parent)
    : QWidget(parent), slicer_(slicer), tasks_(tasks), members_(members) {
    setObjectName("ChaosSlicerPage");

    // TODO: Remove local stylesheet load once ThemeManager loads all slice stylesheets.
    QFile qss(":/themes/light/slicer.qss");
    if (qss.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(QString::fromUtf8(qss.readAll()));
    }

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);

    // Top action bar
    auto* topBar = new QHBoxLayout();
    auto* pageTitle = new QLabel("Chaos Slicer", this);
    QFont titleFont = pageTitle->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    pageTitle->setFont(titleFont);
    topBar->addWidget(pageTitle);

    topBar->addStretch();

    recalculateBtn_ = new QPushButton("Recalculate", this);
    recalculateBtn_->setObjectName("secondaryButton");
    connect(recalculateBtn_, &QPushButton::clicked, this, &ChaosSlicerPage::onRecalculate);
    topBar->addWidget(recalculateBtn_);

    applyBtn_ = new QPushButton("Apply Assignments to Board", this);
    applyBtn_->setObjectName("primaryButton");
    connect(applyBtn_, &QPushButton::clicked, this, &ChaosSlicerPage::onApply);
    topBar->addWidget(applyBtn_);

    mainLayout->addLayout(topBar);

    // Upper section: Sliders on left, Pizza placeholder on right
    auto* upperLayout = new QHBoxLayout();
    upperLayout->setSpacing(16);

    sliders_ = new WeightSlidersPanel(this);
    upperLayout->addWidget(sliders_, 1);

    auto* pizzaCard = new QFrame(this);
    pizzaCard->setObjectName("card");
    auto* cardLayout = new QVBoxLayout(pizzaCard);
    cardLayout->setContentsMargins(12, 12, 12, 12);
    pizza_ = new PizzaWidget(pizzaCard);
    cardLayout->addWidget(pizza_);
    upperLayout->addWidget(pizzaCard, 1);

    mainLayout->addLayout(upperLayout);

    // Lower section: Assignments Table
    table_ = new QTableView(this);
    model_ = new AssignmentTableModel(this);
    table_->setModel(model_);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(AssignmentTableModel::TaskComplexity, QHeaderView::Stretch);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);

    auto allMembers = members_.all();
    table_->setItemDelegateForColumn(AssignmentTableModel::ActionOverride,
                                     new AssigneeDelegate(allMembers, table_));

    mainLayout->addWidget(table_, 1);

    onRecalculate();
}

void ChaosSlicerPage::onRecalculate() {
    try {
        preview_ = slicer_.preview(sliders_->weights());
    } catch (const std::invalid_argument&) {
        return;
    }
    model_->setAssignments(preview_, tasks_.all(), members_.all());
    if (pizza_) {
        pizza_->setPortions(slicer_.portions(preview_));
        pizza_->sliceAnimated();
    }
}

void ChaosSlicerPage::onApply() {
    try {
        slicer_.apply(model_->currentAssignments(), sliders_->weights());
    } catch (const std::invalid_argument&) {
        return;
    }
    refreshFromBoard();
}

void ChaosSlicerPage::refreshFromBoard() {
    if (pizza_) {
        pizza_->setPortions(slicer_.portions());
    }
}

} // namespace planova::slicer
