#pragma once
#include "planova/PlanovaCore.h"
#include "app/CoreBridge.h"
#include <QWidget>

class QListWidget;

class ProjectsHubPage : public QWidget {
    Q_OBJECT
public:
    ProjectsHubPage(PlanovaCore& core, CoreBridge& bridge, QWidget* parent = nullptr);

private:
    void refresh();

    PlanovaCore& core_;
    QListWidget* list_;
};