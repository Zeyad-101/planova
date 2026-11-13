#pragma once
#include "planova/common/Dtos.h"
#include <QStyledItemDelegate>
#include <vector>

namespace planova::slicer {

class AssigneeDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit AssigneeDelegate(const std::vector<MemberDto>& members, QObject* parent = nullptr);
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
private:
    std::vector<MemberDto> members_;
};

} // namespace planova::slicer
