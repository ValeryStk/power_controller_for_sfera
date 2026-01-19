#include <QApplication>
#include <MainWindow.h>
#include "single_application.h"
#include "qrc_files_restorer.h"
#include "config.h"
#include "text_log_constants.h"
#include "logger.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);
    check_single_application_is_running();
    global::mayBe_create_log_dir();
    QrcFilesRestorer::restoreFilesFromQrc(":/4restoring");
    qInstallMessageHandler(myMessageOutput);
    qInfo() << tlc::kStartTheLog;
    MainWindow w;
    w.show();
    return app.exec();
}
