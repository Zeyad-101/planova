#include "ThemeManager.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

ThemeManager& ThemeManager::instance()
{
    static ThemeManager manager;
    return manager;
}

void ThemeManager::apply(ThemeMode)
{
    qApp->setStyle("Fusion");
    QFile file(":/themes/light/base.qss");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not load base.qss from resources";
        return;
    }
    qApp->setStyleSheet(QString::fromUtf8(file.readAll()));
}

QColor ThemeManager::color(Token token) const
{
    return QColor(kLightHex[static_cast<int>(token)]);
}