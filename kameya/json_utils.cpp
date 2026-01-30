#include "json_utils.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace jsn {

bool getJsonObjectFromFile(const QString& path, QJsonObject& object) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "File can't be opened!" << path;
        return false;
    };
    QByteArray data = file.readAll();
    QJsonParseError errorPtr;
    object = QJsonDocument::fromJson(data, &errorPtr).object();
    if (object.isEmpty()) {
        qDebug() << "JSON IS EMPTY: " << errorPtr.errorString();
        return false;
    }
    file.close();
    return true;
}

bool getJsonArrayFromFile(const QString& path, QJsonArray& object) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "File can't be opened!" << path;
        return false;
    };
    QByteArray data = file.readAll();
    QJsonParseError errorPtr;
    object = QJsonDocument::fromJson(data, &errorPtr).array();
    if (object.isEmpty()) {
        qDebug() << "JSON IS EMPTY: " << errorPtr.errorString();
        return false;
    }
    file.close();
    return true;
}

bool saveJsonObjectToFile(const QString& path, const QJsonObject& json_object,
                          QJsonDocument::JsonFormat format) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return false;
    auto json_doc = QJsonDocument(json_object).toJson(format);
    auto result = file.write(json_doc);
    file.close();
    if (result == -1)
        return false;
    else
        return true;
}

bool saveJsonArrayToFile(const QString& path, const QJsonArray& json_object,
                         QJsonDocument::JsonFormat format) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return false;
    auto json_doc = QJsonDocument(json_object).toJson(format);
    auto result = file.write(json_doc);
    file.close();
    if (result == -1)
        return false;
    else
        return true;
}

}  // namespace jsn
