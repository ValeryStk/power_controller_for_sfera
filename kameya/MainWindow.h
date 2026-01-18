#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <QSettings>
#include <QMessageBox>
#include "power_supply_manager.h"
#include "Sounder.h"
#include <QCloseEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <graphics_items/bulbs_item.h>
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


private:
    Ui::MainWindow      *ui;
    PowerSupplyManager  *m_powerManager;
    QGraphicsScene      *m_sceneCalibr;
    BulbsQGraphicsItem          *m_bulbs_graphics_item;
    QShortcut           *repeatLastNotification;
    QShortcut           *open_log_dir;
    QShortcut           *show_log;
    Sounder             m_sounder;

    void initializeVariables();
    void createObjects();
    void setUpGui();
    void setUpScene();
    void showMessageBox  (QMessageBox::Icon ico,
                          QString titleText,
                          QString text);
    bool checkSafetyUser();

    int    m_current_lamp_index;
    QVector<double> bandProfileChannels;
    QVector<QLabel*> lamps;
    QHash <int, QString> m_pages;


private:
    void operation_failed_voice_notification();
    void retest_all_powers();

private slots:
    void testSlot();
    void openRootFolder();
    void update_ps( int ps,
                    int out,
                    bool isOn,
                    double voltage,
                    double current);

    void on_pushButton_Backward_clicked();
    void on_pushButton_Forward_clicked();
    void on_checkBox_cooling_for_big_sphere_stateChanged(int arg1);

    void on_pushButton_switchOffOneLamp_clicked();
    void on_pushButton_switch_on_one_lamp_clicked();
    void on_comboBox__mode_currentIndexChanged(int index);
    void switch_on_all_lamps();
    void switch_off_all_lamps();
    void on_pushButton_sound_toggled(bool checked);
    void on_pushButton_update_power_states_clicked();
    void on_pushButton_update_clicked();
    void on_pushButton_open_log_clicked();
};
#endif // MAIN_WINDOW_H
