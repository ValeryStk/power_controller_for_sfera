#include <MainWindow.h>

#include <QApplication>

#include "config.h"
#include "logger.h"
#include "qrc_files_restorer.h"
#include "qsharedmemory.h"
#include "qsystemsemaphore.h"
#include "text_log_constants.h"

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);

    QSystemSemaphore semaphore("<POWER_CONTROLLER>", 1);
    semaphore.acquire();
    QSharedMemory sharedMemory("<POWER_CONTROLLER 2>");
    bool is_running;
    if (sharedMemory.attach()) {
        is_running = true;
    } else {
        sharedMemory.create(1);
        is_running = false;
    }
    semaphore.release();
    if (is_running) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(
            "Приложение уже запущено.\nВы можете запустить только один "
            "экземпляр приложения.");
        msgBox.exec();
        return EXIT_SUCCESS;
    }

    global::mayBe_create_log_dir();
    qInstallMessageHandler(myMessageOutput);
    qInfo() << tlc::kStartTheLog;
    utils::restoreFilesFromDefaultQrc();
    MainWindow w;
    w.show();
    return app.exec();
}
