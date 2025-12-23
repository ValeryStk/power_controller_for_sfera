#ifndef WAVESHIFTFINDER_H
#define WAVESHIFTFINDER_H

#include <QObject>

class WaveShiftFinder:public QObject
{
    Q_OBJECT
public:
    WaveShiftFinder();
//private:
    void readFramesFromJSON();
};

#endif // WAVESHIFTFINDER_H
