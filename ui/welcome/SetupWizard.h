#pragma once
#include "planova/PlanovaCore.h"
#include <QWizard>

class SetupWizard : public QWizard {
    Q_OBJECT
public:
    explicit SetupWizard(PlanovaCore& core, QWidget* parent = nullptr);
    void accept() override;

private:
    PlanovaCore& core_;
};