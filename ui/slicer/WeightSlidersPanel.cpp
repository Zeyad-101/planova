#include "slicer/WeightSlidersPanel.h"
#include "slicer/WeightRebalance.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

namespace planova::slicer {

WeightSlidersPanel::WeightSlidersPanel(QWidget* parent) : QWidget(parent) {
    setObjectName("card");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 16, 20, 16);
    mainLayout->setSpacing(14);

    // Header row: Card Title on left, Sum label on right
    auto* headerLayout = new QHBoxLayout();
    auto* cardTitle = new QLabel("Assignment Strategy Weights", this);
    cardTitle->setObjectName("cardTitle");
    headerLayout->addWidget(cardTitle);
    headerLayout->addStretch();

    sumLabel_ = new QLabel("Sum: 100%", this);
    sumLabel_->setObjectName("sumLabel");
    headerLayout->addWidget(sumLabel_);
    mainLayout->addLayout(headerLayout);

    // Horizontal sliders row across the card
    auto* slidersRow = new QHBoxLayout();
    slidersRow->setSpacing(24);

    const char* names[3] = {"Skill Match (α)", "Workload Leveling (β)", "Priority Weight (γ)"};
    for (int i = 0; i < 3; ++i) {
        auto* itemLayout = new QHBoxLayout();
        itemLayout->setSpacing(10);

        auto* nameLabel = new QLabel(names[i], this);
        nameLabel->setObjectName("sliderNameLabel");
        itemLayout->addWidget(nameLabel);

        sliders_[i] = new QSlider(Qt::Horizontal, this);
        sliders_[i]->setRange(0, 100);
        sliders_[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        itemLayout->addWidget(sliders_[i], 1);

        valueLabels_[i] = new QLabel(this);
        valueLabels_[i]->setObjectName("sliderValueLabel");
        itemLayout->addWidget(valueLabels_[i]);

        connect(sliders_[i], &QSlider::valueChanged, this, [this, i](int v) {
            onSliderMoved(i, v);
        });

        slidersRow->addLayout(itemLayout, 1);
    }

    mainLayout->addLayout(slidersRow);
    syncWidgets();
}

void WeightSlidersPanel::setWeights(const ScoringWeights& w) {
    if (!w.valid()) return;
    weights_ = w;
    syncWidgets();
}

void WeightSlidersPanel::onSliderMoved(int index, int value) {
    weights_ = rebalance(weights_, index, value);
    syncWidgets();
    emit weightsChanged(weights_);
}

void WeightSlidersPanel::syncWidgets() {
    const int values[3] = {weights_.skill, weights_.workload, weights_.priority};
    for (int i = 0; i < 3; ++i) {
        QSignalBlocker blocker(sliders_[i]);  // avoid recursion while we move the other sliders
        sliders_[i]->setValue(values[i]);
        valueLabels_[i]->setText(QString::number(values[i]) + "%");
    }
    sumLabel_->setText(QString("Sum: %1%").arg(weights_.skill + weights_.workload + weights_.priority));
}

} // namespace planova::slicer
