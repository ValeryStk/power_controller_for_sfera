#ifndef QRC_FILES_RESTORER_H
#define QRC_FILES_RESTORER_H

class QString;

/*!
    \brief Класс является объектом, необходимым для восстановления файлов в
   директории с СПО из ресурсов в случае их отсутствия.

    Данный класс предназначен для восстановления файлов в директорию с СПО из
   ресурсов в случае их отсутствия.
*/
class QrcFilesRestorer {
public:
    static void restoreFilesFromQrc(QString path2Qrc);
};

#endif  // #include "qtextcodec.h"
