#pragma once
#include "planova/common/Dtos.h"
#include "planova/slicer/SlicerTypes.h"
#include <QAbstractTableModel>
#include <vector>

namespace planova::slicer {

class AssignmentTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column { TaskComplexity = 0, Skill, Priority, Assignee, Rationale, Status, ActionOverride, ColCount };

    explicit AssignmentTableModel(QObject* parent = nullptr);
    void setAssignments(const std::vector<Assignment>& assignments, const std::vector<TaskDto>& tasks,
                        const std::vector<MemberDto>& members);
    std::vector<Assignment> currentAssignments() const { return assignments_; }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    const TaskDto* findTask(const Id& id) const;
    const MemberDto* findMember(const Id& id) const;
    std::vector<Assignment> assignments_;
    std::vector<TaskDto> tasks_;
    std::vector<MemberDto> members_;
};

} // namespace planova::slicer
