#include <MainWindow.h>

#include <QApplication>

#include "config.h"
#include "logger.h"
#include "qrc_files_restorer.h"
#include "single_application.h"
#include "text_log_constants.h"

int main(int argc, char *argv[]) {
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
