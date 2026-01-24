#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QCloseEvent>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QShortcut>
#include <QThread>
#include <QTimer>

#include "graphics_items/bulbs_item.h"
#include "power_supply_manager.h"
#include "sounder.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
    friend class bulbs_states_UnitTests;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    PowerSupplyManager *m_powerManager;
    QThread *m_powers_manager_thread;
    QGraphicsScene *m_sceneCalibr;
    BulbsQGraphicsItem *m_bulbs_graphics_item;
    QTimer *m_timer_to_update_power_states;

    QShortcut *repeatLastNotification;
    QShortcut *show_log;
    Sounder m_sounder;

    void initializeVariables();
    void createObjects();
    void setUpGui();
    void setUpScene();
    void showMessageBox(QMessageBox::Icon ico, QString titleText, QString text);
    bool checkSafetyUser();

    int m_current_lamp_index;
    QVector<double> bandProfileChannels;
    QVector<QLabel *> lamps;
    QHash<int, QString> m_pages;

private:
    void operation_failed_voice_notification();
    void retest_all_powers();

private slots:
    void testSlot();
    void update_lamp_state(int lamp_index, double voltage, double current);
    void update_ps(int ps, int out, bool isOn, double voltage, double current);

    void on_pushButton_Forward_clicked();
    void on_checkBox_cooling_for_big_sphere_stateChanged(int arg1);
    void on_comboBox_mode_currentIndexChanged(int index);
    void on_pushButton_sound_toggled(bool checked);
    void on_pushButton_update_power_states_clicked();
    void on_pushButton_update_clicked();
    void on_pushButton_open_log_clicked();

    void switch_on_all_lamps();
    void switch_off_all_lamps();
    void on_pushButton_switchOffOneLamp_clicked();
    void on_pushButton_switch_on_one_lamp_clicked();
signals:
    void make_one_lamp_on(int);
};
#endif  // MAIN_WINDOW_H
