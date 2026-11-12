#pragma once
#include "planova/PlanovaCore.h"
#include <QObject>
#include <QString>

class CoreBridge : public QObject {
    Q_OBJECT
public:
    explicit CoreBridge(PlanovaCore& core, QObject* parent = nullptr);
    ~CoreBridge() override;

signals:
    void changed(const QString& eventType);

private:
    PlanovaCore& core_;
    int token_ = 0;
};