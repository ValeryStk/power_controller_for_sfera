#include <QApplication>
#include <MainWindow.h>
#include "QrcFilesRestorer.h"
#include "config.h"
#include "qsharedmemory.h"
#include "qsystemsemaphore.h"
#include "qtextcodec.h"
#include "text_log_constants.h"

void myMessageOutput(QtMsgType type,
                     const QMessageLogContext& context,
                     const QString& msg) {

  Q_UNUSED(context);
  QFile file(QCoreApplication::applicationDirPath() + global::path_to_log_dir + "/logic.log");
  if (file.exists())
    file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
  else
    file.open(QIODevice::WriteOnly | QIODevice::Text);
  QString time = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("CP 1251"));
    QString OutMessage = "";
    QTextStream out(&file);

    switch (type) {
      case QtInfoMsg:
        OutMessage = msg+"\n";
        break;
      case QtDebugMsg:
        OutMessage = QString("Debug[%1]: %2\n").arg(time, msg);
        break;
      case QtWarningMsg:
        OutMessage = QString("Warning[%1]: %2\n").arg(time, msg);
        break;
      case QtCriticalMsg:
        OutMessage = QString("Critical[%1]: %2\n").arg(time, msg);
        break;
      case QtFatalMsg:
        OutMessage = QString("Fatal[%1]: %2\n").arg(time, msg);
        abort();
    }
    out << OutMessage;
    file.close();
}


int main(int argc, char *argv[])
{

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
      msgBox.setText("Приложение уже запущено.\nВы можете запустить только один экземпляр приложения.");
      msgBox.exec();
      return 0;
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QrcFilesRestorer::restoreFilesFromQrc(":/4restoring");
    QApplication app(argc, argv);
    qInstallMessageHandler(myMessageOutput);
    qInfo() << tlc::kStartTheLog;
    MainWindow w;
    w.show();
    return app.exec();
}
