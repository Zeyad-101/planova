#pragma once
#include <QJsonObject>
#include <cstdint>
#include <string>

struct Event {
    std::string id;
    int64_t ts;
    std::string author;
    std::string type;
    QJsonObject payload;
};