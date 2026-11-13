#include "slicer/AssignmentTableModel.h"
#include "planova/slicer/SlicerNames.h"
#include "theme/ThemeManager.h"
#include <QColor>

namespace planova::slicer {

AssignmentTableModel::AssignmentTableModel(QObject* parent)
    : QAbstractTableModel(parent) {}

void AssignmentTableModel::setAssignments(const std::vector<Assignment>& assignments,
                                          const std::vector<TaskDto>& tasks,
                                          const std::vector<MemberDto>& members) {
    beginResetModel();
    assignments_ = assignments;
    tasks_ = tasks;
    members_ = members;
    endResetModel();
}

int AssignmentTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(assignments_.size());
}

int AssignmentTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return Column::ColCount;
}

const TaskDto* AssignmentTableModel::findTask(const Id& id) const {
    for (const auto& t : tasks_) {
        if (t.id == id) return &t;
    }
    return nullptr;
}

const MemberDto* AssignmentTableModel::findMember(const Id& id) const {
    for (const auto& m : members_) {
        if (m.id == id) return &m;
    }
    return nullptr;
}

QVariant AssignmentTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(assignments_.size())) {
        return {};
    }

    const auto& a = assignments_[index.row()];
    const auto* t = findTask(a.taskId);
    const auto* m = findMember(a.memberId);

    switch (index.column()) {
        case Column::TaskComplexity:
            if (role == Qt::DisplayRole && t) {
                return QString("%1 (%2 pts)").arg(QString::fromStdString(t->title)).arg(t->complexity);
            }
            break;
        case Column::Skill:
            if (role == Qt::DisplayRole && t) {
                return QString::fromStdString(skillToString(t->skill));
            }
            break;
        case Column::Priority:
            if (role == Qt::DisplayRole && t) {
                return QString::fromStdString(priorityToString(t->priority));
            }
            break;
        case Column::Assignee:
            if (role == Qt::DisplayRole) {
                QString mName = m ? QString::fromStdString(m->name) : QString::fromStdString(a.memberId);
                if (a.matchPct > 0) {
                    return QString("%1 · %2%").arg(mName).arg(a.matchPct);
                }
                return mName;
            }
            break;
        case Column::Rationale:
            if (role == Qt::DisplayRole) {
                return QString::fromStdString(a.rationale);
            }
            break;
        case Column::Status:
            if (role == Qt::DisplayRole) {
                return a.waitingOnDep ? QString("Waiting on Dep") : QString("Ready");
            }
            if (role == Qt::BackgroundRole && a.waitingOnDep) {
                return QVariant::fromValue(ThemeManager::instance().color(Token::WarningSoft));
            }
            if (role == Qt::ForegroundRole && a.waitingOnDep) {
                return QVariant::fromValue(ThemeManager::instance().color(Token::TextPrimary));
            }
            break;
        case Column::ActionOverride:
            if (role == Qt::DisplayRole) {
                return m ? QString::fromStdString(m->name) : QString::fromStdString(a.memberId);
            }
            if (role == Qt::EditRole) {
                return QString::fromStdString(a.memberId);
            }
            break;
    }
    return {};
}

bool AssignmentTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(assignments_.size())) {
        return false;
    }
    if (index.column() != Column::ActionOverride || role != Qt::EditRole) {
        return false;
    }

    std::string newMemberId = value.toString().toStdString();
    if (!findMember(newMemberId)) {
        return false;
    }

    auto& a = assignments_[index.row()];
    a.memberId = newMemberId;
    a.matchPct = 0;
    a.rationale = "Manual override";

    emit dataChanged(this->index(index.row(), 0), this->index(index.row(), Column::ColCount - 1));
    return true;
}

Qt::ItemFlags AssignmentTableModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == Column::ActionOverride) {
        f |= Qt::ItemIsEditable;
    }
    return f;
}

QVariant AssignmentTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case Column::TaskComplexity: return QString("Task & Complexity");
            case Column::Skill: return QString("Required Skill");
            case Column::Priority: return QString("Priority");
            case Column::Assignee: return QString("Recommended Assignee");
            case Column::Rationale: return QString("Match Rationale");
            case Column::Status: return QString("Status");
            case Column::ActionOverride: return QString("Action Override");
            default: break;
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

} // namespace planova::slicer
