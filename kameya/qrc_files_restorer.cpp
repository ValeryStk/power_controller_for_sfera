#include "qrc_files_restorer.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>

namespace utils {

extern const char defaultRestoreQrcPath[] = ":/4restoring";

void restoreFilesFromQrc(const QString &path2Qrc) {
    const QString application_path = QApplication::applicationDirPath() + "/";
    QDir dir(path2Qrc);
    QStringList files = dir.entryList(QDir::Files);

    for (int i = 0; i < files.count(); ++i) {
        QString filenameNew = application_path + files.at(i);
        QFile file(filenameNew);
        if (!file.exists()) {
            QString restored_file = path2Qrc + "/" + files.at(i);
            qDebug() << "restored file: " << restored_file;
            qDebug() << "restore result: "
                     << QFile::copy(restored_file, filenameNew);
            file.setPermissions(QFileDevice::WriteUser | QFileDevice::ReadUser);
        }
    }
}

void restoreFilesFromDefaultQrc() {
    restoreFilesFromQrc(defaultRestoreQrcPath);
}

}  // namespace utils
