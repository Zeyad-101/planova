#include "slicer/PizzaWidget.h"
#include "theme/ThemeManager.h"
#include <QEasingCurve>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QVariantAnimation>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace planova::slicer {

namespace {
QString extractInitials(const std::string& name) {
    QString qname = QString::fromStdString(name).trimmed();
    if (qname.isEmpty()) return {};
    auto parts = qname.split(' ', Qt::SkipEmptyParts);
    if (parts.isEmpty()) return {};
    if (parts.size() == 1) return parts[0].left(1).toUpper();
    return (parts.first().left(1) + parts.last().left(1)).toUpper();
}
}

PizzaWidget::PizzaWidget(QWidget* parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(280, 260);

    anim_ = new QVariantAnimation(this);
    anim_->setDuration(400);
    anim_->setStartValue(0.0);
    anim_->setKeyValueAt(0.5, 1.0);
    anim_->setEndValue(0.0);
    anim_->setEasingCurve(QEasingCurve::OutQuad);
    connect(anim_, &QVariantAnimation::valueChanged, this, [this](const QVariant& v) {
        explode_ = v.toDouble();
        update();
    });
}

void PizzaWidget::setPortions(const std::vector<planova::slicer::Portion>& portions) {
    portions_ = portions;
    update();
}

void PizzaWidget::sliceAnimated() {
    if (anim_->state() == QVariantAnimation::Running) {
        anim_->stop();
    }
    anim_->start();
}

void PizzaWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (portions_.empty()) {
        p.setPen(ThemeManager::instance().color(Token::TextMuted));
        p.drawText(rect(), Qt::AlignCenter, "No workload data");
        return;
    }

    const int legendRows = static_cast<int>((portions_.size() + 1) / 2);
    const int rowHeight = 22;
    const int legendHeight = legendRows * rowHeight + 12;

    const int availableHeight = height() - legendHeight - 16;
    const int pieDiameter = std::max(60, std::min(width() - 32, availableHeight));
    const double radius = pieDiameter / 2.0;

    const QPointF center(width() / 2.0, 16.0 + radius);
    const QRectF basePieRect(center.x() - radius, center.y() - radius, pieDiameter, pieDiameter);

    // 1. Draw slices
    for (size_t i = 0; i < portions_.size(); ++i) {
        const auto& portion = portions_[i];
        QColor sliceColor = ThemeManager::instance().color(
            static_cast<Token>(static_cast<int>(Token::Member1) + (i % 6)));

        double span = portion.thinSlice ? 3.0 : portion.spanAngle;
        if (span <= 0.0) continue;

        double bisectorDeg = portion.startAngle + span / 2.0;
        double rad = bisectorDeg * M_PI / 180.0;
        double offsetDist = explode_ * 8.0;
        double dx = offsetDist * std::sin(rad);
        double dy = -offsetDist * std::cos(rad);

        QRectF sliceRect = basePieRect.translated(dx, dy);

        int qtStart = static_cast<int>(std::round((90.0 - portion.startAngle) * 16.0));
        int qtSpan = static_cast<int>(std::round(-span * 16.0));

        p.setPen(Qt::NoPen);
        p.setBrush(sliceColor);
        p.drawPie(sliceRect, qtStart, qtSpan);

        if (!portion.thinSlice && portion.spanAngle >= 20.0) {
            double centroidRadius = radius * (2.0 / 3.0);
            double cx = center.x() + dx + centroidRadius * std::sin(rad);
            double cy = center.y() + dy - centroidRadius * std::cos(rad);

            QString initials = extractInitials(portion.memberName);
            p.setPen(ThemeManager::instance().color(Token::AccentText));
            QFont font = p.font();
            font.setBold(true);
            font.setPointSize(std::max(8, std::min(11, static_cast<int>(radius / 8.0))));
            p.setFont(font);

            QRectF textRect(cx - 20, cy - 12, 40, 24);
            p.drawText(textRect, Qt::AlignCenter, initials);
        }
    }

    // 2. Draw Legend below
    const int legendTop = static_cast<int>(center.y() + radius + 14);
    const int colWidth = width() / 2;

    QFont legendFont = p.font();
    legendFont.setBold(false);
    legendFont.setPointSize(9);
    p.setFont(legendFont);

    for (size_t i = 0; i < portions_.size(); ++i) {
        const auto& portion = portions_[i];
        QColor dotColor = ThemeManager::instance().color(
            static_cast<Token>(static_cast<int>(Token::Member1) + (i % 6)));

        int col = static_cast<int>(i % 2);
        int row = static_cast<int>(i / 2);

        int x = (col == 0) ? 20 : (colWidth + 10);
        int y = legendTop + row * rowHeight;

        p.setPen(Qt::NoPen);
        p.setBrush(dotColor);
        p.drawEllipse(QPointF(x + 5, y + 6), 5, 5);

        p.setPen(ThemeManager::instance().color(Token::TextPrimary));
        QString label = QString("%1: %2 tasks (%3 pts)")
                            .arg(QString::fromStdString(portion.memberName))
                            .arg(portion.taskCount)
                            .arg(portion.totalComplexity);

        p.drawText(QRect(x + 16, y, colWidth - 26, rowHeight),
                   Qt::AlignLeft | Qt::AlignVCenter, label);
    }
}

} // namespace planova::slicer
