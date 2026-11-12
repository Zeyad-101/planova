#pragma once
#include "CoreBridge.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(PlanovaCore& core, CoreBridge& bridge, QWidget* parent = nullptr);

private:
    void buildPages();
    QWidget* buildSidebar();

    PlanovaCore& core_;
    CoreBridge& bridge_;
    QStackedWidget* pages_;
};