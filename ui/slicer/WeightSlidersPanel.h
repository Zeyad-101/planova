#pragma once
#include "planova/slicer/ScoringWeights.h"
#include <QWidget>

class QLabel;
class QSlider;

namespace planova::slicer {

class WeightSlidersPanel : public QWidget {
    Q_OBJECT
public:
    explicit WeightSlidersPanel(QWidget* parent = nullptr);
    ScoringWeights weights() const { return weights_; }
    void setWeights(const ScoringWeights& w);
signals:
    void weightsChanged(const planova::slicer::ScoringWeights& weights);
private:
    void onSliderMoved(int index, int value);
    void syncWidgets();
    ScoringWeights weights_;
    QSlider* sliders_[3] = {nullptr, nullptr, nullptr};
    QLabel* valueLabels_[3] = {nullptr, nullptr, nullptr};
    QLabel* sumLabel_ = nullptr;
};

} // namespace planova::slicer
