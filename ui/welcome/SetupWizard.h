#pragma once
#include "planova/PlanovaCore.h"
#include <QVector>
#include <QWidget>

class QButtonGroup;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QStackedWidget;

class SetupWizard : public QWidget {
    Q_OBJECT
public:
    SetupWizard(PlanovaCore& core, QWidget* parent = nullptr);
    void reset();

signals:
    void finished();

private:
    void goToStep(int step);
    void updateNavigation();
    void updateReview();
    void finish();

    PlanovaCore& core_;
    QStackedWidget* pages_;
    QVector<QLabel*> stepCircles_;
    QVector<QLabel*> stepNames_;
    QLabel* stepBadge_ = nullptr;
    QLabel* summary_ = nullptr;
    QLineEdit* name_ = nullptr;
    QLineEdit* team_ = nullptr;
    QPlainTextEdit* idea_ = nullptr;
    QLineEdit* stack_ = nullptr;
    QButtonGroup* modelGroup_ = nullptr;
    QPushButton* previous_ = nullptr;
    QPushButton* next_ = nullptr;
    int current_ = 0;
};