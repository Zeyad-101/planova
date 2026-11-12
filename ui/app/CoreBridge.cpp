#include "CoreBridge.h"

CoreBridge::CoreBridge(PlanovaCore& core, QObject* parent)
    : QObject(parent), core_(core)
{
    token_ = core_.bus().subscribe([this](const std::string& eventType) {
        emit changed(QString::fromStdString(eventType));
    });
}

CoreBridge::~CoreBridge()
{
    core_.bus().unsubscribe(token_);
}