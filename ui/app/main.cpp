#include "app/CoreBridge.h"
#include "app/MainWindow.h"
#include "planova/PlanovaCore.h"
#include "theme/ThemeManager.h"

#include <QApplication>
#include <QStandardPaths>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(planova);
    ThemeManager::instance().apply(ThemeMode::Light);

    const QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    PlanovaCore core(std::filesystem::path(documents.toStdString()) / "Planova");
    CoreBridge bridge(core);
    MainWindow window(core, bridge);
    window.show();

    return app.exec();
}