#ifndef LOGGER_H
#define LOGGER_H

#include "qtextcodec.h"
#include "QFile"
#include "QCoreApplication"
#include "config.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext& context,
                     const QString& msg) {
    Q_UNUSED(context);
    QFile file(QCoreApplication::applicationDirPath() +
               global::path_to_logs_dir + "/logic.log");
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
            OutMessage = msg + "\n";
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

#endif  // CONFIG_H
