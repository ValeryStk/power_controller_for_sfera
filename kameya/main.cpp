#include <QApplication>
#include <MainWindow.h>
#include "QrcFilesRestorer.h"

int main(int argc, char *argv[])
{

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QrcFilesRestorer::restoreFilesFromQrc(":/4restoring");
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
