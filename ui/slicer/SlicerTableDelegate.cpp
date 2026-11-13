#include "slicer/SlicerTableDelegate.h"
#include "slicer/AssignmentTableModel.h"
#include "theme/ThemeManager.h"
#include <QFont>
#include <QFontMetrics>
#include <QPainter>

namespace planova::slicer {

SlicerTableDelegate::SlicerTableDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

QSize SlicerTableDelegate::sizeHint(const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const {
    switch (index.column()) {
        case AssignmentTableModel::TaskComplexity: return QSize(260, 56);
        case AssignmentTableModel::Skill: return QSize(110, 56);
        case AssignmentTableModel::Priority: return QSize(95, 56);
        case AssignmentTableModel::Assignee: return QSize(180, 56);
        case AssignmentTableModel::Rationale: return QSize(240, 56);
        case AssignmentTableModel::Status: return QSize(135, 56);
        case AssignmentTableModel::ActionOverride: return QSize(140, 56);
        default: return QSize(option.rect.width(), 56);
    }
}

void SlicerTableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                const QModelIndex& index) const {
    if (index.column() == AssignmentTableModel::ActionOverride) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    auto& tm = ThemeManager::instance();
    const QRect rect = option.rect;

    // Row Background
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(rect, tm.color(Token::AccentSoft));
    } else {
        painter->fillRect(rect, tm.color(Token::Surface));
    }

    // Divider line at bottom
    painter->setPen(QPen(tm.color(Token::SurfaceAlt), 1));
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());

    const int col = index.column();

    if (col == AssignmentTableModel::TaskComplexity) {
        // Line 1: Title
        QString title = index.data(AssignmentTableModel::TaskTitleRole).toString();
        QFont titleFont = painter->font();
        titleFont.setPointSize(10);
        titleFont.setBold(true);
        painter->setFont(titleFont);
        painter->setPen(tm.color(Token::TextPrimary));
        painter->drawText(QRect(rect.left() + 12, rect.top() + 8, rect.width() - 20, 20),
                          Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, title);

        // Line 2: 5 dots + C<level>
        int complexity = index.data(AssignmentTableModel::TaskComplexityRole).toInt();
        int dotY = rect.top() + 36;
        int dotX = rect.left() + 14;

        for (int d = 0; d < 5; ++d) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(d < complexity ? tm.color(Token::Member5) : tm.color(Token::Track));
            painter->drawEllipse(QPointF(dotX + d * 10, dotY), 3, 3);
        }

        QFont cFont = painter->font();
        cFont.setPointSize(9);
        cFont.setBold(false);
        painter->setFont(cFont);
        painter->setPen(tm.color(Token::TextMuted));
        painter->drawText(QRect(dotX + 54, dotY - 8, 40, 16),
                          Qt::AlignLeft | Qt::AlignVCenter, QString("C%1").arg(complexity));

    } else if (col == AssignmentTableModel::Skill) {
        QString skill = index.data(AssignmentTableModel::RequiredSkillRole).toString();
        QFont font = painter->font();
        font.setPointSize(9);
        font.setBold(true);
        painter->setFont(font);
        QFontMetrics fm(font);

        int pillW = fm.horizontalAdvance(skill) + 16;
        int pillH = 22;
        QRectF pillRect(rect.left() + 8, rect.top() + (rect.height() - pillH) / 2.0, pillW, pillH);

        painter->setPen(QPen(tm.color(Token::Accent), 1));
        painter->setBrush(tm.color(Token::AccentSoft));
        painter->drawRoundedRect(pillRect, 4, 4);

        painter->setPen(tm.color(Token::Accent));
        painter->drawText(pillRect, Qt::AlignCenter, skill);

    } else if (col == AssignmentTableModel::Priority) {
        QString prio = index.data(AssignmentTableModel::PriorityRole).toString();
        QFont font = painter->font();
        font.setPointSize(9);
        font.setBold(true);
        painter->setFont(font);
        QFontMetrics fm(font);

        int pillW = fm.horizontalAdvance(prio) + 16;
        int pillH = 22;
        QRectF pillRect(rect.left() + 8, rect.top() + (rect.height() - pillH) / 2.0, pillW, pillH);

        QColor bg = tm.color(Token::SurfaceAlt);
        QColor textCol = tm.color(Token::TextMuted);
        QColor border = tm.color(Token::Border);

        if (prio == "CRITICAL") {
            bg = tm.color(Token::DangerSoft);
            textCol = tm.color(Token::Member1);
            border = tm.color(Token::DangerSoft);
        } else if (prio == "HIGH") {
            bg = tm.color(Token::AccentSoft);
            textCol = tm.color(Token::Accent);
            border = tm.color(Token::Accent);
        } else if (prio == "MEDIUM") {
            bg = tm.color(Token::StatusBar);
            textCol = tm.color(Token::TextMuted);
        }

        painter->setPen(QPen(border, 1));
        painter->setBrush(bg);
        painter->drawRoundedRect(pillRect, 4, 4);

        painter->setPen(textCol);
        painter->drawText(pillRect, Qt::AlignCenter, prio);

    } else if (col == AssignmentTableModel::Assignee) {
        QString initials = index.data(AssignmentTableModel::AssigneeInitialsRole).toString();
        QString name = index.data(AssignmentTableModel::AssigneeNameRole).toString();
        int matchPct = index.data(AssignmentTableModel::MatchPctRole).toInt();

        int cy = rect.top() + rect.height() / 2;
        int avatarX = rect.left() + 16;

        // Avatar circle
        painter->setPen(Qt::NoPen);
        painter->setBrush(tm.color(Token::TextPrimary));
        painter->drawEllipse(QPointF(avatarX, cy), 11, 11);

        QFont avFont = painter->font();
        avFont.setPointSize(8);
        avFont.setBold(true);
        painter->setFont(avFont);
        painter->setPen(tm.color(Token::AccentText));
        painter->drawText(QRectF(avatarX - 11, cy - 11, 22, 22), Qt::AlignCenter, initials);

        // Name
        QFont nameFont = painter->font();
        nameFont.setPointSize(10);
        nameFont.setBold(false);
        painter->setFont(nameFont);
        QFontMetrics fm(nameFont);
        int nameW = fm.horizontalAdvance(name);

        painter->setPen(tm.color(Token::TextPrimary));
        painter->drawText(QRect(avatarX + 16, cy - 10, nameW, 20),
                          Qt::AlignLeft | Qt::AlignVCenter, name);

        // Match Pct Badge
        if (matchPct > 0) {
            QString pctStr = QString("%1%").arg(matchPct);
            QFont pctFont = painter->font();
            pctFont.setPointSize(9);
            pctFont.setBold(true);
            painter->setFont(pctFont);
            QFontMetrics pfm(pctFont);

            int pillW = pfm.horizontalAdvance(pctStr) + 12;
            int pillH = 18;
            int pillX = avatarX + 22 + nameW;
            QRectF pillRect(pillX, cy - pillH / 2.0, pillW, pillH);

            painter->setPen(Qt::NoPen);
            painter->setBrush(tm.color(Token::SuccessSoft));
            painter->drawRoundedRect(pillRect, 9, 9);

            painter->setPen(tm.color(Token::TextPrimary));
            painter->drawText(pillRect, Qt::AlignCenter, pctStr);
        }

    } else if (col == AssignmentTableModel::Rationale) {
        QString rat = index.data(AssignmentTableModel::RationaleRole).toString();
        QFont font = painter->font();
        font.setPointSize(9);
        painter->setFont(font);
        painter->setPen(tm.color(Token::TextPrimary));
        painter->drawText(rect.adjusted(10, 6, -10, -6),
                          Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, rat);

    } else if (col == AssignmentTableModel::Status) {
        bool waiting = index.data(AssignmentTableModel::WaitingOnDepRole).toBool();
        QString text = waiting ? QString::fromUtf8("\u25B2 Waiting on Dep") : "Ready";

        QFont font = painter->font();
        font.setPointSize(9);
        font.setBold(true);
        painter->setFont(font);
        QFontMetrics fm(font);

        int pillW = fm.horizontalAdvance(text) + 16;
        int pillH = 22;
        QRectF pillRect(rect.left() + 8, rect.top() + (rect.height() - pillH) / 2.0, pillW, pillH);

        QColor bg = waiting ? tm.color(Token::WarningSoft) : tm.color(Token::SuccessSoft);
        QColor textCol = waiting ? tm.color(Token::TextPrimary) : tm.color(Token::Member3);

        painter->setPen(Qt::NoPen);
        painter->setBrush(bg);
        painter->drawRoundedRect(pillRect, 11, 11);

        painter->setPen(textCol);
        painter->drawText(pillRect, Qt::AlignCenter, text);
    }

    painter->restore();
}

} // namespace planova::slicer
