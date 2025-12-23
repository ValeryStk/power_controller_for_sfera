#include "WaveShiftFinder.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QFile>
#include <QDebug>

WaveShiftFinder::WaveShiftFinder()
{

}

void WaveShiftFinder::readFramesFromJSON()
{

    /*QJsonObject m_process_params = readJsonDocumentFromFile(QDir::currentPath()+"/calibr_process.json").object();
    QJsonObject obj =  m_process_params.find("vrs")->toObject();
    m_process.visibleSensor.grade = obj.value("grade").toInt();
    m_process.visibleSensor.slit =  obj.value("slit").toInt();
    QJsonArray arr = obj.value("lambdas").toArray();
    for(const auto &it:qAsConst(arr))m_process.visibleSensor.lambdas.push_back(it.toInt())*/


    QFile file(":/4restoring/waveFrames.json");
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    qDebug()<<"Data size: "<<data.size();
    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);
    file.close();
    if (doc.isNull()) {
        qDebug() << "Ошибка разбора JSON!";
        return;
    }
    QJsonObject obj =  doc.object().find("hg")->toObject();
    QStringList lamps = obj.keys();
    for(int i=0;i<lamps.size();++i){
        qDebug()<<" ------------------------ "<<lamps[i];
    }
}
