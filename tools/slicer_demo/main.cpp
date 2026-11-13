#include "theme/ThemeManager.h"
#include <QApplication>
#include <QMainWindow>
#include <QStyleFactory>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    ThemeManager::instance().apply(ThemeMode::Light);

    QMainWindow window;
    window.setWindowTitle("Planova Chaos Slicer (dev harness)");
    window.resize(1200, 800);
    window.show();
    return app.exec();
}
