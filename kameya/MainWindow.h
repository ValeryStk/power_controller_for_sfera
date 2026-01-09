#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QShortcut>
#include <QSettings>
#include <QMessageBox>
#include "power_supply_manager.h"
#include "Sounder.h"
#include "QThread"
#include <QCloseEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <graphics_items/OpticTable.h>
#include "Types.h"
#include "QTimer"


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
    QString             rootDir;
    QTimer              m_afterLampsOnHeatingTimer;

    Sounder             m_sounder;
    PowerSupplyManager *m_powerManager;


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
    void setUpScene();
    void makeConnects();
    void showMessageBox  (QMessageBox::Icon ico, QString titleText, QString text);
    bool checkSafetyUser();
    void setSavingPath();

    bool   isStop = false;
    double maxProfileValue;
    double minProfileValue;
    int    m_lampsCounter;

    QVector<double> bandProfileChannels;
    QVector<QLabel*> lamps;
    QHash <int, QString> m_pages;

    QGraphicsScene *m_sceneCalibr;
    OpticTable *ot;
    LampsNumber m_lampsNumber;


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
    void on_checkBox_5_cooling_for_big_sphere_stateChanged(int arg1);

    void showElapsedHeatingTime();
    void openFolderInExplorer();
    void afterLampWasSwitchedOff();
    void on_pushButton_switchOffOneLamp_clicked();

    void on_pushButton_switch_on_one_lamp_clicked();
    void on_comboBox__mode_currentIndexChanged(int index);
    void switch_on_all_lamps();
    void switch_off_all_lamps();
};
#endif // MAINWINDOW_H
