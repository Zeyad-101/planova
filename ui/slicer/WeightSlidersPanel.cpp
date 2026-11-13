#include "slicer/WeightSlidersPanel.h"
#include "slicer/WeightRebalance.h"
#include <QGridLayout>
#include <QLabel>
#include <QSlider>

namespace planova::slicer {

WeightSlidersPanel::WeightSlidersPanel(QWidget* parent) : QWidget(parent) {
    setObjectName("WeightSlidersPanel");
    auto* grid = new QGridLayout(this);
    const char* names[3] = {"Skill Match", "Workload Balance", "Priority / Velocity"};
    for (int i = 0; i < 3; ++i) {
        sliders_[i] = new QSlider(Qt::Horizontal, this);
        sliders_[i]->setRange(0, 100);
        valueLabels_[i] = new QLabel(this);
        grid->addWidget(new QLabel(names[i], this), i, 0);
        grid->addWidget(sliders_[i], i, 1);
        grid->addWidget(valueLabels_[i], i, 2);
        connect(sliders_[i], &QSlider::valueChanged, this, [this, i](int v) { onSliderMoved(i, v); });
    }
    sumLabel_ = new QLabel(this);
    sumLabel_->setObjectName("sumLabel");
    grid->addWidget(sumLabel_, 3, 0, 1, 3);
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
