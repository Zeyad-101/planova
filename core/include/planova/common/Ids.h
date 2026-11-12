#pragma once
#include <QJsonObject>
#include <cstdint>
#include <string>

std::string makeEventId(int64_t ts, const std::string& author, const QJsonObject& payload);