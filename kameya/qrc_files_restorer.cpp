#include "qrc_files_restorer.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>

namespace utils {

extern const char defaultRestoreQrcPath[] = ":/4restoring";

void restoreFilesFromQrc(const QString &path2Qrc) {
    const QString application_path = QApplication::applicationDirPath() + "/";
    qDebug() << "QApplication dir path: " << application_path;
    QDir dir(path2Qrc);
    QStringList files = dir.entryList(QDir::Files);

    for (int i = 0; i < files.count(); ++i) {
        QString filenameNew = application_path + files.at(i);
        qDebug() << "file: " << filenameNew;
        QFile file(filenameNew);
        if (!file.exists()) {
            QFile::copy(path2Qrc + "/" + files.at(i), filenameNew);
            file.setPermissions(QFileDevice::WriteUser | QFileDevice::ReadUser);
        }
    }
}

}  // namespace utils
