#include "planova/common/Ids.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QString>

std::string makeEventId(int64_t ts, const std::string& author, const QJsonObject& payload)
{
    QByteArray body = QString::fromStdString(author).toUtf8()
    + QJsonDocument(payload).toJson(QJsonDocument::Compact);
    QString hash = QString::fromLatin1(
                       QCryptographicHash::hash(body, QCryptographicHash::Sha256).toHex()).left(8);
    return std::to_string(ts) + "-" + hash.toStdString();
}