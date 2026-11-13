#include "slicer/AssigneeDelegate.h"
#include <QComboBox>

namespace planova::slicer {

AssigneeDelegate::AssigneeDelegate(const std::vector<MemberDto>& members, QObject* parent)
    : QStyledItemDelegate(parent), members_(members) {}

QWidget* AssigneeDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/,
                                        const QModelIndex& /*index*/) const {
    auto* combo = new QComboBox(parent);
    for (const auto& m : members_) {
        combo->addItem(QString::fromStdString(m.name), QString::fromStdString(m.id));
    }
    return combo;
}

void AssigneeDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    auto* combo = qobject_cast<QComboBox*>(editor);
    if (!combo) return;

    QString currentId = index.data(Qt::EditRole).toString();
    int idx = combo->findData(currentId);
    if (idx >= 0) {
        combo->setCurrentIndex(idx);
    }
}

void AssigneeDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                    const QModelIndex& index) const {
    auto* combo = qobject_cast<QComboBox*>(editor);
    if (!combo || !model) return;

    model->setData(index, combo->currentData(), Qt::EditRole);
}

} // namespace planova::slicer
