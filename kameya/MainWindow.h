#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QShortcut>
#include <QSettings>
#include <QMessageBox>
#include "PowerSupplyManager.h"
#include "QCustomPlot.h"
#include "Sounder.h"
#include "QThread"
#include <QCloseEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <graphics_items/OpticTable.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void closeEvent(QCloseEvent*event)override;

protected:
    bool eventFilter(QObject*obj,QEvent*event)override;

private:
    Ui::MainWindow     *ui;
    TestResults         m_testsResults;
    SensorType          m_activeSensor;
    logicCalibr         m_logic;
    SpectralGetting     m_gettingSpecs;
    QString             rootDir;

    QSettings          *m_ini;
    QTimer              m_opticalParamsChangigTimer;
    QTimer              m_afterLampsOnHeatingTimer;

    ShutterText         m_shutterText;
    Sounder             m_sounder;
    QMovie              m_movie;
    PowerSupplyManager *m_powerManager;

    QStringList m_expositionsList;
    QMap<QString, QVector<QMap<QString,double>>> m_expositions_for_grade1200;
    QMap<QString, QVector<QMap<QString,double>>> m_expositions_for_grade300;
    QJsonObject expositions_for_grade_1200;
    QJsonObject expositions_for_grade_300;
    bool m_isTestPassed;
    bool m_isGraphUpdate;
    bool m_isControlPressed;
    bool m_isResetZero;
    bool m_isAngleReached;
    int  m_infraRedChannelsNumber;
    int  m_visibleRangeChannelsNumber;
    int  m_darkModeCapturingDelay;
    quint64 total_number_of_spectrs;
    quint64 captured_spectrs_counter;

    void initializeVariables();
    void createObjects();
    void setUpGui();
    void setUpGraph(QCustomPlot *widget);
    void setUpScene();
    void makeConnects();
    void showMessageBox  (QMessageBox::Icon ico, QString titleText, QString text);
    void createTreeFolders();
    bool checkSafetyUser();
    void startMeasuringAfterGatheringExpositions();
    void setSavingPath();
    void setMetaData();
    void changeWave();
    double getExpositionFromJson();

    bool   isStop = false;
    double maxProfileValue;
    double minProfileValue;
    int    m_lampsCounter;

    QVector<double> bandProfileChannels;
    QVector<QLabel*> lamps;
    QHash <int, QString> m_pages;
    QHash <int,int> grades;

    int expoIndex;
    double evaluate_exposition;
    source_expositions src_expositions;
    QMap<double,QString> Angles;

    QGraphicsScene *m_sceneCalibr;
    QGraphicsScene *m_scenePrepare;
    OpticTable *ot;
    SourcesNames srcNames;

    SensorNames sensorNames;
    LampsNumber m_lampsNumber;
    QVector<double> m_angles;
    AnglesScene m_anglesScene;
    calibrParams m_process;
    solarTasksResults solarResults;
    void getProcessParams();
    void getExpositions();
    void showSpectralPlot(QCustomPlot* cp, SensorType st, QVector<double>* data, double max, double min);
    void getFilterIndex(int &filterIndex, const int waveLength);
    QJsonDocument readJsonDocumentFromFile(const QString &docName);
    QShortcut *repeatLastNotification;

public slots:
void timeOutCaseHandler();

private slots:
    void testSlot();
    void openRootFolder();

    void on_pushButton_Backward_clicked();
    void on_pushButton_Forward_clicked();
    void on_pushButton_angleBigSphere_clicked();
    void on_pushButton_angleSmallSphere_clicked();
    void on_pushButton_angleLasers_clicked();
    void on_pushButton_angleRightSafety_clicked();
    void on_pushButton_zero_clicked();
    void on_checkBox_5_cooling_for_big_sphere_stateChanged(int arg1);

    void showElapsedHeatingTime();
    void anglePositionIsReached();

    void showShutterState();
    void showFilterState();
    void showGratingState();
    void showCurrentWavelength();
    void getSingleSpectr4ActiveSensor();

    void openFolderInExplorer();

    void afterDarkReady();
    void checkShutterState();
    void saveAnglesToJson();
    void continueCalibrProcess();
    void afterLampWasSwitchedOff();
    void on_pushButton_switchOffOneLamp_clicked();


signals:
    void requestVisibleSpectr();
    void requestInfraredSpectr();
};
#endif // MAINWINDOW_H
