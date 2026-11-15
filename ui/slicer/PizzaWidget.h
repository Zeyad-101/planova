#pragma once
#include "planova/slicer/SlicerTypes.h"
#include <QWidget>
#include <vector>

class QVariantAnimation;

namespace planova::slicer {

class PizzaWidget : public QWidget {
    Q_OBJECT
public:
    explicit PizzaWidget(QWidget* parent = nullptr);
    void setPortions(const std::vector<planova::slicer::Portion>& portions);
    void sliceAnimated();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<planova::slicer::Portion> portions_;
    double explode_ = 0.0;
    QVariantAnimation* anim_ = nullptr;
};

} // namespace planova::slicer
