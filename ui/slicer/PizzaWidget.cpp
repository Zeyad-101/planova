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
    setMinimumSize(280, 360);

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

    auto& tm = ThemeManager::instance();

    const double radius = 120.0;
    const double pieDiameter = radius * 2.0;
    const QPointF center(width() / 2.0, 16.0 + radius);
    const QRectF basePieRect(center.x() - radius, center.y() - radius, pieDiameter, pieDiameter);

    // 1. Draw slices with crust, toppings, borders and member avatar badges
    for (size_t i = 0; i < portions_.size(); ++i) {
        const auto& portion = portions_[i];
        QColor sliceColor = tm.color(static_cast<Token>(static_cast<int>(Token::Member1) + (i % 6)));

        double span = portion.thinSlice ? 3.0 : portion.spanAngle;
        if (span <= 0.0) continue;

        double bisectorDeg = portion.startAngle + span / 2.0;
        double rad = bisectorDeg * M_PI / 180.0;
        double offsetDist = explode_ * 12.0;
        double dx = offsetDist * std::sin(rad);
        double dy = -offsetDist * std::cos(rad);

        QRectF sliceRect = basePieRect.translated(dx, dy);

        int qtStart = static_cast<int>(std::round((90.0 - portion.startAngle) * 16.0));
        int qtSpan = static_cast<int>(std::round(-span * 16.0));

        // Draw base slice
        p.setPen(QPen(tm.color(Token::TextPrimary), 1.5));
        p.setBrush(sliceColor);
        p.drawPie(sliceRect, qtStart, qtSpan);

        // Draw crust outer rim
        p.setPen(QPen(tm.color(Token::Border), 8.0, Qt::SolidLine, Qt::FlatCap));
        p.setBrush(Qt::NoBrush);
        p.drawArc(sliceRect.adjusted(4, 4, -4, -4), qtStart, qtSpan);

        if (!portion.thinSlice && portion.spanAngle >= 25.0) {
            // Toppings: pepperoni circles
            double p1Rad = (portion.startAngle + span * 0.35) * M_PI / 180.0;
            double p1Dist = radius * 0.72;
            double px1 = center.x() + dx + p1Dist * std::sin(p1Rad);
            double py1 = center.y() + dy - p1Dist * std::cos(p1Rad);
            p.setPen(QPen(tm.color(Token::TextPrimary), 1.0));
            p.setBrush(tm.color(Token::Member1));
            p.drawEllipse(QPointF(px1, py1), 7, 7);

            double p2Rad = (portion.startAngle + span * 0.72) * M_PI / 180.0;
            double p2Dist = radius * 0.76;
            double px2 = center.x() + dx + p2Dist * std::sin(p2Rad);
            double py2 = center.y() + dy - p2Dist * std::cos(p2Rad);
            p.drawEllipse(QPointF(px2, py2), 6, 6);

            // Toppings: basil green dots
            double bRad = (portion.startAngle + span * 0.52) * M_PI / 180.0;
            double bDist = radius * 0.84;
            p.setPen(Qt::NoPen);
            p.setBrush(tm.color(Token::Member3));
            p.drawEllipse(QPointF(center.x() + dx + bDist * std::sin(bRad),
                                  center.y() + dy - bDist * std::cos(bRad)), 3, 3);

            // Center avatar circle with member initials
            double centroidRadius = radius * 0.50;
            double cx = center.x() + dx + centroidRadius * std::sin(rad);
            double cy = center.y() + dy - centroidRadius * std::cos(rad);

            QString initials = extractInitials(portion.memberName);
            p.setPen(QPen(tm.color(Token::Surface), 2.0));
            p.setBrush(tm.color(Token::TextPrimary));
            p.drawEllipse(QPointF(cx, cy), 13, 13);

            p.setPen(tm.color(Token::AccentText));
            QFont font = p.font();
            font.setBold(true);
            font.setPointSize(9);
            p.setFont(font);

            QRectF textRect(cx - 13, cy - 13, 26, 26);
            p.drawText(textRect, Qt::AlignCenter, initials);
        }
    }

    // 2. Draw Legend below as horizontal centered pill tags
    const int legendTop = static_cast<int>(center.y() + radius + 22);

    QFont legendFont = p.font();
    legendFont.setBold(false);
    legendFont.setPointSize(9);
    p.setFont(legendFont);
    QFontMetrics fm(legendFont);

    struct PillInfo {
        QString text;
        QColor dotColor;
        int width;
    };
    std::vector<PillInfo> pills;
    pills.reserve(portions_.size());

    for (size_t i = 0; i < portions_.size(); ++i) {
        const auto& portion = portions_[i];
        QColor dotColor = tm.color(static_cast<Token>(static_cast<int>(Token::Member1) + (i % 6)));
        QString text = QString("%1  %2 tasks")
                           .arg(QString::fromStdString(portion.memberName))
                           .arg(portion.taskCount);
        int w = fm.horizontalAdvance(text) + 32;
        pills.push_back({text, dotColor, w});
    }

    // Arrange pills into rows that fit widget width
    std::vector<std::vector<PillInfo>> rows;
    std::vector<PillInfo> currentRow;
    int currentWidth = 0;
    const int pillSpacing = 10;
    const int maxRowWidth = width() - 40;

    for (const auto& pill : pills) {
        if (!currentRow.empty() && currentWidth + pillSpacing + pill.width > maxRowWidth) {
            rows.push_back(currentRow);
            currentRow.clear();
            currentWidth = 0;
        }
        currentRow.push_back(pill);
        currentWidth += (currentRow.size() == 1 ? 0 : pillSpacing) + pill.width;
    }
    if (!currentRow.empty()) {
        rows.push_back(currentRow);
    }

    int y = legendTop;
    const int pillHeight = 26;
    for (const auto& row : rows) {
        int rowTotalWidth = 0;
        for (size_t k = 0; k < row.size(); ++k) {
            rowTotalWidth += row[k].width + (k > 0 ? pillSpacing : 0);
        }
        int startX = (width() - rowTotalWidth) / 2;
        int x = startX;

        for (const auto& pill : row) {
            QRectF pillRect(x, y, pill.width, pillHeight);
            p.setPen(QPen(tm.color(Token::Border), 1.0));
            p.setBrush(tm.color(Token::Surface));
            p.drawRoundedRect(pillRect, 13, 13);

            // Dot
            p.setPen(Qt::NoPen);
            p.setBrush(pill.dotColor);
            p.drawEllipse(QPointF(x + 11, y + 13), 4, 4);

            // Text
            p.setPen(tm.color(Token::TextPrimary));
            p.drawText(QRectF(x + 20, y, pill.width - 24, pillHeight),
                       Qt::AlignLeft | Qt::AlignVCenter, pill.text);

            x += pill.width + pillSpacing;
        }
        y += pillHeight + 8;
    }
}

} // namespace planova::slicer
