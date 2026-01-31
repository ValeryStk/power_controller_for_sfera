#ifndef QRC_FILES_RESTORER_H
#define QRC_FILES_RESTORER_H

class QString;

namespace utils {

extern const char defaultRestoreQrcPath[];

void restoreFilesFromQrc(const QString &path2Qrc);

}  // end namespace utils

#endif  // QRC_FILES_RESTORER_H
