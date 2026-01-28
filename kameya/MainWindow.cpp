#include "MainWindow.h"

#include <QDateTime>
#include <QDir>
#include <QGraphicsScene>
#include <QPair>
#include <QProcess>
#include <QStyle>
#include <QTimer>
#include <chrono>
#include <memory>

#include "QStyleFactory"
#include "Version.h"
#include "math.h"
#include "qmutex.h"
#include "ui_MainWindow.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <synchapi.h>

#include <QDesktopServices>
#include <QMetaType>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>

#include "config.h"
#include "graphics_items/power_supply_item.h"
#include "icon_generator.h"
#include "text_log_constants.h"

PowerUnitParams initial_struct;
Q_DECLARE_METATYPE(QVector<PowerUnitParams>)
PowerUnitParams powers_outs_states[NUMBER_OF_LAMPS];

constexpr char kSwitchOnAllLampsText[] = "Включить все лампы";
constexpr char kSwitchOnOneLampText[] = "Включить одну лампу";
constexpr char kSwitchOffLampsText[] = "Выключить все лампы";
constexpr char kSwitchOffOneLampText[] = "Выключить одну лампу";

constexpr int MAX_Z_INDEX = 1000;

lamps_powers_config cfg;

namespace {

void openFileByDefaultSoftware(const QString& filePath) {
    QUrl url = QUrl::fromLocalFile(filePath);
    QDesktopServices::openUrl(url);
}

void append_v_i_to_log(const QString& filePath, const QString& line) {
    static QMutex mutex;
    QtConcurrent::run([line, filePath]() {
        QMutexLocker locker(&mutex);
        QFile file(filePath);
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << line << "\n";
        }
    });
};

}  // namespace

QVector<QPair<PowerSupplyItem*, QString>> psis;
std::map<int, std::function<void()>> set_active_power_out;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    qRegisterMetaType<QVector<PowerUnitParams>>();
    global::mayBe_create_log_dir();
    global::get_config_struct(cfg);
    createObjects();
    initializeVariables();
    setUpGui();
    setUpScene();
    qApp->installEventFilter(this);
    m_sounder.muteSoundNotifications(!cfg.is_sound);
    ui->pushButton_sound->setChecked(!cfg.is_sound);
    m_sounder.playSound("safety.mp3");

    m_timer_to_update_power_states = new QTimer(this);
    m_timer_to_update_power_states->setInterval(POWER_SUPPLIES_UPDATE_INTERVAL);

    connect(m_timer_to_update_power_states, &QTimer::timeout, this, [this]() {
        for (int i = 0; i < NUMBER_OF_LAMPS; ++i) {
            bool isConnected = m_powerManager->isPowerOutConnected(i);
            PowerUnitParams result;
            int power_num = global::get_power_num_by_index(i);
            int out_num = global::get_power_out_by_index(i);
            if (isConnected) {
                result = m_powerManager->get_all_params_for_lamp_out(i);
                QString v_i_line =
                    QString("%1 %2 %3 %4")
                        .arg(i + 1)
                        .arg(QDateTime::currentDateTime().toString(
                            "yyyy_MM_dd_hh:mm:ss"))
                        .arg(result.V)
                        .arg(result.I);
                QString full_path_to_cv_log =
                    QApplication::applicationDirPath() +
                    global::relative_path_to_cv_log_file;
                append_v_i_to_log(full_path_to_cv_log, v_i_line);
            }
            update_ps(power_num, out_num, result.isOn, result.V, result.I);
        }
        m_sceneCalibr->update();
    });

    connect(this, SIGNAL(make_one_lamp_on(int)), m_powerManager,
            SIGNAL(make_one_lamp_on(int)));
    connect(this, SIGNAL(make_one_lamp_off(int)), m_powerManager,
            SIGNAL(make_one_lamp_off(int)));
    connect(m_powerManager, SIGNAL(lamp_state_changed(int, double, double)),
            this, SLOT(update_lamp_state(int, double, double)));
    connect(m_powerManager, SIGNAL(test_finished(QVector<PowerUnitParams>)),
            this, SLOT(testSlot(QVector<PowerUnitParams>)));
    connect(m_powerManager,
            SIGNAL(lamp_state_changed_to_ub(int, double, double, bool)),
            SLOT(handle_undone_process(int, double, double, bool)));
    connect(this, SIGNAL(make_all_lamps_off()), m_powerManager,
            SIGNAL(make_all_lamps_off()));
    connect(this, SIGNAL(make_all_lamps_on()), m_powerManager,
            SIGNAL(make_all_lamps_on()));
    connect(this, SIGNAL(test_all()), m_powerManager, SIGNAL(test_all()));
    connect(m_powerManager, SIGNAL(update_power_out(int, double, double)), this,
            SLOT(update_ps_out(int, double, double)));
    connect(m_powerManager, SIGNAL(process_interrupted_by_user()), this,
            SLOT(handle_interrupted_process()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::closeEvent(QCloseEvent* event) {
    event->ignore();
    m_powerManager->stopFlagForAll_Lamps.store(true);
    m_powers_manager_thread->quit();
    m_timer_to_update_power_states->stop();
    for (int i = 0; i < psis.size(); ++i) {
        m_sceneCalibr->removeItem(psis[i].first);
        delete psis[i].first;
    }
    m_sceneCalibr->removeItem(m_bulbs_graphics_item);
    delete m_bulbs_graphics_item;
    m_powerManager->deleteLater();
    QApplication::processEvents();
    event->accept();
}

void MainWindow::showMessageBox(QMessageBox::Icon ico, QString titleText,
                                QString text) {
    if (text.isEmpty()) text = "Нет данных";
    QMessageBox msgBox;
    msgBox.setIcon(ico);
    msgBox.setText(titleText);
    msgBox.setInformativeText(text);
    msgBox.exec();
}

bool MainWindow::checkSafetyUser() {
    if (ui->checkBox_1_uv->isChecked() && ui->checkBox_2_power->isChecked() &&
        ui->checkBox_cooling_for_big_sphere->isChecked())
        return true;
    else
        return false;
}

void MainWindow::setActivePowerOut() {
    for (int i = 0; i < psis.size(); ++i) {
        psis[i].first->set_all_outs_unactive();
    }
    set_active_power_out[m_current_lamp_index]();
}

void MainWindow::operation_failed_voice_notification() {
    QTimer::singleShot(
        1000, this, [this]() { m_sounder.playSound("operation_failed.mp3"); });
    showMessageBox(QMessageBox::Warning, "Ошибка",
                   "Операция провалилась, смотрите logic.log");
}

void MainWindow::retest_all_powers() {
    qInfo() << tlc::kOperationUpdateAllPowersStates;
    ui->label_TitlePage->setText(tlc::kStateMachineUpdateAllLampsCommandState);
    m_state = CONTROLLER_STATES::UPDATE_ALL_STATES_PROCESS;
    emit test_all();
}

void MainWindow::showWaitFinishWarning() {
    showMessageBox(QMessageBox::Information, "Контроллер занят",
                   "Дождитесь выполнения операции или отмените текущую.");
}

void MainWindow::update_ps(int ps, int out, bool isOn, double voltage,
                           double current) {
    if (ps > NUMBER_OF_POWER_SUPPLIES || ps < 0) return;
    PowerSupplyItem* ps_item = nullptr;
    for (int i = 0; i < psis.size(); ++i) {
        psis[i].first->set_all_outs_unactive();
    }
    ps_item = psis[ps - 1].first;

    if (ps_item == nullptr) return;

    if (out == 1) {
        ps_item->set_voltage_out_1(voltage);
        ps_item->set_current_out_1(current);
        ps_item->set_enabled_out_1(isOn);
    }

    if (out == 2) {
        ps_item->set_voltage_out_2(voltage);
        ps_item->set_current_out_2(current);
        ps_item->set_enabled_out_2(isOn);
    }

    // m_current_lamp_index
    int current_pwr_num = global::get_power_num_by_index(m_current_lamp_index);
    int active_out = global::get_power_out_by_index(m_current_lamp_index);
    ps_item = psis[current_pwr_num - 1].first;
    if (active_out == 1) {
        ps_item->set_out_1_active();
    }
    if (active_out == 2) {
        ps_item->set_out_2_active();
    }
}

void MainWindow::update_ps_out(int index, double voltage, double current) {
    int pn = global::get_power_num_by_index(index);
    int po = global::get_power_out_by_index(index);
    update_ps(pn, po, true, voltage, current);
}

void MainWindow::handle_undone_process(int index, double voltage,
                                       double current, bool is_on) {
    qWarning() << "!!!!!!!!!!!! UNDONE COMMAND FOR LAMP " << index + 1
               << "!!!!!!!!!!!!";
    m_bulbs_graphics_item->setBulbUndefined(index);
    m_bulbs_graphics_item->set_current_lamp_index(index);

    int pwr_num = global::get_power_num_by_index(index);
    int pwr_out = global::get_power_out_by_index(index);
    update_ps(pwr_num, pwr_out, voltage, current, is_on);
    m_current_lamp_index = index;
    setActivePowerOut();
    m_sceneCalibr->update();
    if (m_state == CONTROLLER_STATES::ONE_LAMP_SWITCH_OFF_PROCESS ||
        m_state == CONTROLLER_STATES::ONE_LAMP_SWITCH_OFF_PROCESS ||
        ((m_state == CONTROLLER_STATES::ALL_LAMPS_SWITCH_OFF_PROCESS) &&
         (m_current_lamp_index == MIN_CURRENT_LAMP_INDEX)) ||
        ((m_state == CONTROLLER_STATES::ALL_LAMPS_SWITCH_ON_PROCESS) &&
         (m_current_lamp_index == MAX_CURRENT_LAMP_INDEX))) {
        ui->label_TitlePage->setText(tlc::kStateMachineWaitCommandState);
        m_state = CONTROLLER_STATES::WAIT_COMMAND;
        m_sounder.playSound("lamp_in_undefined_state_is_founded.mp3");
    }
}

void MainWindow::handle_interrupted_process() {
    qWarning() << "PROCESS WAS INTERRUPTED BY USER";
    m_powerManager->stopFlagForAll_Lamps.store(false);
    m_powerManager->stopFlagForOne_Lamp.store(false);
    m_state = CONTROLLER_STATES::WAIT_COMMAND;
    ui->label_TitlePage->setText(tlc::kStateMachineWaitCommandState);
    m_sounder.playSound("process_was_cancelled.mp3");
    showMessageBox(QMessageBox::Information, "Стоп", "Операция прервана.");
}

void MainWindow::testSlot(QVector<PowerUnitParams> powers_outs_states) {
    qDebug() << "---- ALL POWERS TEST RESULT ----";
    if (powers_outs_states.size() < NUMBER_OF_LAMPS) {
        qCritical() << "UNEXPECTED RESULT (NUMBER OF POWER PARAMS LESS THAN "
                       "NUMBER OF LAMPS (6))";
        showMessageBox(QMessageBox::Icon::Critical, "критическая ошибка",
                       "Размер параметров для блоков питания не совпадает с "
                       "количеством ламп.");
        return;
    }
    bool first_power_state = powers_outs_states[0].isOn;
    bool second_power_state = powers_outs_states[2].isOn;
    bool third_power_state = powers_outs_states[4].isOn;

    bulb_state bulbs_states[NUMBER_OF_LAMPS];
    bool power_states[NUMBER_OF_POWER_SUPPLIES] = {
        first_power_state, second_power_state, third_power_state};

    for (int i = 0; i < NUMBER_OF_LAMPS; ++i) {
        auto json_current_limit_value = cfg.lamps_array[i].max_current;
        double current_max_value = powers_outs_states[i].Ilim;
        double current_present_value = powers_outs_states[i].I;
        double current_voltage = powers_outs_states[i].V;
        bool isOn = powers_outs_states[i].isOn;

        int power_num = global::get_power_num_by_index(i);
        int out_num = global::get_power_out_by_index(i);
        bool is_connected = power_states[power_num - 1];

        if ((current_voltage <= global::kVoltageZeroAccuracy) && is_connected) {
            bulbs_states[i] = bulb_state::OFF;
        } else if (qAbs(json_current_limit_value - current_present_value) <
                   global::kCurrentTargetAccuracy) {
            bulbs_states[i] = bulb_state::ON;
        } else {
            bulbs_states[i] = bulb_state::UNDEFINED;
        }
        QString current_limit_message =
            "FACT CURRENT LIMIT %1 POWER %2 OUT %3 --> LAMP %4";
        qDebug() << current_limit_message.arg(current_max_value)
                        .arg(power_num)
                        .arg(out_num)
                        .arg(i + 1);
        if (out_num == 1) {
            psis[power_num - 1].first->set_max_current_out_1(current_max_value);
            psis[power_num - 1].first->set_current_out_1(current_present_value);
            psis[power_num - 1].first->set_voltage_out_1(current_voltage);
            psis[power_num - 1].first->set_enabled_out_1(isOn);
        } else if (out_num == 2) {
            psis[power_num - 1].first->set_max_current_out_2(current_max_value);
            psis[power_num - 1].first->set_current_out_2(current_present_value);
            psis[power_num - 1].first->set_voltage_out_2(current_voltage);
            psis[power_num - 1].first->set_enabled_out_2(isOn);
        }
    }

    m_bulbs_graphics_item->set_bulb_states(bulbs_states);
    m_sceneCalibr->update();

    QVector<QPair<QLabel*, bool>> labels = {
        {ui->label_test_power_1, first_power_state},
        {ui->label_test_power_2, second_power_state},
        {ui->label_test_power_3, third_power_state}};
    bool testOk = false;
    if (first_power_state && second_power_state && third_power_state) {
        testOk = true;
    }
    for (int i = 0; i < labels.size(); ++i) {
        QString style = "";
        if (labels[i].second)
            style = "background:rgb(0,128,0)";
        else {
            style = "background:rgb(128,0,0)";
            testOk = false;
        }
        labels[i].first->setStyleSheet(style);
    }
    if (testOk) {
        m_sounder.playSound("testOk.mp3");
        ui->pushButton_Forward->setEnabled(true);
        qInfo() << "ALL POWERS TEST IS OK.";
    } else {
        m_sounder.playSound("network_error.mp3");
        ui->pushButton_Forward->setEnabled(cfg.is_unclock);
        qWarning() << "ALL POWERS TEST IS FAILED.";
        qInfo() << "POWER 1: " << first_power_state;
        qInfo() << "POWER 2: " << second_power_state;
        qInfo() << "POWER 3: " << third_power_state;
        showMessageBox(QMessageBox::Warning, "Тест не пройден",
                       "Блоки питания не готовы к работе.");
    }
    ui->pushButton_update_power_states->setEnabled(true);
    ui->pushButton_update->setEnabled(true);
    m_state = CONTROLLER_STATES::WAIT_COMMAND;
    ui->label_TitlePage->setText(tlc::kStateMachineWaitCommandState);
}

void MainWindow::initializeVariables() {
    QStringList pages = {"Техника безопасности", "Тестирование", "Калибровка"};
    for (int i = 0; i < pages.size(); ++i) m_pages.insert(i, pages.at(i));

    m_current_lamp_index = MAX_CURRENT_LAMP_INDEX;
}

void MainWindow::createObjects() {
    m_powerManager = new PowerSupplyManager;
    m_powers_manager_thread = new QThread;
    m_powerManager->moveToThread(m_powers_manager_thread);
    connect(m_powers_manager_thread, &QThread::started, m_powerManager,
            &PowerSupplyManager::initSocket);
    connect(m_powers_manager_thread, &QThread::finished, m_powerManager,
            &QObject::deleteLater);
    m_powers_manager_thread->start();

    m_sceneCalibr = new QGraphicsScene;

    repeatLastNotification = new QShortcut(this);
    show_log = new QShortcut(this);

    repeatLastNotification->setKey(Qt::CTRL + Qt::Key_R);
    show_log->setKey(Qt::CTRL + Qt::Key_L);

    connect(repeatLastNotification, SIGNAL(activated()), &m_sounder,
            SLOT(playLastSound()));
    connect(show_log, SIGNAL(activated()), this,
            SLOT(on_pushButton_open_log_clicked()));
    connect(ui->pushButton_repeat_last_sound, SIGNAL(clicked()), &m_sounder,
            SLOT(playLastSound()));
}

void MainWindow::setUpGui() {
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setAttribute(Qt::AA_Use96Dpi);
    QString appTitle = "POWER CONTROLLER ";
    appTitle.append(VER_PRODUCTVERSION_STR);
    setWindowTitle(appTitle);
    ui->label_TitlePage->setText(m_pages.value(0));
    ui->centralwidget->setStyleSheet(
        "background-color:#2E2F30;color:lightgrey");
    ui->pushButton_Backward->setVisible(false);
    ui->stackedWidget->setCurrentIndex(0);
    ui->comboBox_mode->addItem("Все лампы");
    for (int i = 0; i < NUMBER_OF_LAMPS; ++i) {
        QString str_color = cfg.lamps_array[i].color;
        QColor color(str_color);
        auto icon = iut::createIcon(color.red(), color.green(), color.blue());
        ui->comboBox_mode->addItem(icon, cfg.lamps_array[i].name);
    }
    ui->pushButton_switchOffOneLamp->setIcon(QIcon(":/svg/trending_down.svg"));
    ui->pushButton_switchOffOneLamp->setIconSize(QSize(64, 64));
    ui->pushButton_switchOffOneLamp->setText(kSwitchOffLampsText);

    ui->pushButton_switch_on_one_lamp->setIcon(QIcon(":/svg/trending_up.svg"));
    ui->pushButton_switch_on_one_lamp->setIconSize(QSize(64, 64));
    ui->pushButton_switch_on_one_lamp->setText(kSwitchOnAllLampsText);

    ui->pushButton_update_power_states->setIcon(QIcon(":/svg/update.svg"));
    ui->pushButton_update_power_states->setIconSize(QSize(64, 64));

    ui->pushButton_update->setIcon(QIcon(":/svg/update.svg"));
    ui->pushButton_update->setIconSize(QSize(64, 64));

    ui->pushButton_sound->setIcon(QIcon(":/svg/volume_up.svg"));
    ui->pushButton_sound->setIconSize(QSize(64, 64));

    ui->pushButton_open_log->setIcon(QIcon(":/svg/log.svg"));
    ui->pushButton_open_log->setIconSize(QSize(64, 64));
}

void MainWindow::setUpScene() {
    ui->graphicsView->setScene(m_sceneCalibr);
    psis = {{nullptr, global::kJsonKeyPowerSupply1_Object},
            {nullptr, global::kJsonKeyPowerSupply2_Object},
            {nullptr, global::kJsonKeyPowerSupply3_Object}};
    const QString svg_power_path = ":/svg/PS.svg";

    for (int i = 0; i < psis.size(); ++i) {
        psis[i].first = new PowerSupplyItem(svg_power_path, cfg.powers[i].name,
                                            psis[i].second);
        m_sceneCalibr->addItem(psis[i].first);
    }

    for (int i = 0; i < NUMBER_OF_LAMPS; ++i) {
        auto color = QColor(cfg.lamps_array[i].color);
        int power_index = global::get_power_num_by_index(i) - 1;
        int out_number = global::get_power_out_by_index(i);

        psis[power_index].first->setScale(0.8);
        if (out_number == 1) {
            psis[power_index].first->set_out_1_color(color);
            set_active_power_out[i] = [=]() {
                psis[power_index].first->set_out_1_active();
            };
        } else {
            psis[power_index].first->set_out_2_color(color);
            set_active_power_out[i] = [=]() {
                psis[power_index].first->set_out_2_active();
            };
        }
    }
    m_sceneCalibr->setSceneRect(0, 0, 1000, 600);
    m_bulbs_graphics_item = new BulbsQGraphicsItem;
    m_bulbs_graphics_item->setZValue(MAX_Z_INDEX);
    m_sceneCalibr->addItem(m_bulbs_graphics_item);
    m_sceneCalibr->update();
    setActivePowerOut();
}

// GUI handlers
void MainWindow::on_pushButton_Forward_clicked() {
    static bool isEnd = false;
    if (isEnd) return;
    if (ui->stackedWidget->currentIndex() < ui->stackedWidget->count() - 1) {
        if (m_pages.value(ui->stackedWidget->currentIndex()) ==
            "Техника безопасности") {
            if (!checkSafetyUser()) {
                m_sounder.playSound("checkWarnings.mp3");
                return;
            }
        }

        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() +
                                           1);

        if (m_pages.value(ui->stackedWidget->currentIndex()) ==
            "Техника безопасности") {
            m_sounder.playSound("safety.mp3");
            return;
        }
        if (m_pages.value(ui->stackedWidget->currentIndex()) ==
            "Тестирование") {
            ui->pushButton_Forward->setEnabled(true);
            ui->label_TitlePage->setText(
                tlc::kStateMachineUpdateAllLampsCommandState);
            return;
        }
        if (m_pages.value(ui->stackedWidget->currentIndex()) == "Калибровка") {
            // m_sounder.playSound("startCalibration.mp3");
            isEnd = true;
            ui->pushButton_Forward->setVisible(false);
        }
    }
}

void MainWindow::on_pushButton_open_log_clicked() {
    auto pathToLogicLog = QApplication::applicationDirPath() +
                          global::relative_path_to_logic_log_file;
    openFileByDefaultSoftware(pathToLogicLog);
}

void MainWindow::on_checkBox_cooling_for_big_sphere_stateChanged(int arg1) {
    if (arg1 == Qt::Checked) {
        m_sounder.playSound("dontForgetCooling.mp3");
    };
}

void MainWindow::on_comboBox_mode_currentIndexChanged(int index) {
    static bool is_first_start_index = true;
    if (is_first_start_index) {
        m_current_lamp_index = MAX_CURRENT_LAMP_INDEX;
        is_first_start_index = false;
        return;
    }

    if (index == 0) {
        ui->pushButton_switchOffOneLamp->setText(kSwitchOffLampsText);
        ui->pushButton_switch_on_one_lamp->setText(kSwitchOnAllLampsText);
    } else {
        ui->pushButton_switchOffOneLamp->setText(kSwitchOffOneLampText);
        ui->pushButton_switch_on_one_lamp->setText(kSwitchOnOneLampText);
    }

    if (index < 1 || index > 6) return;
    m_current_lamp_index = index - 1;

    m_bulbs_graphics_item->set_current_lamp_index(m_current_lamp_index);

    m_sceneCalibr->update();
    setActivePowerOut();
}

void MainWindow::on_pushButton_sound_toggled(bool checked) {
    if (checked) {
        m_sounder.playSound("audioNotificationsOff.mp3");
        m_sounder.muteSoundNotifications(checked);
        ui->pushButton_sound->setIcon(QIcon(":/svg/no_sound.svg"));

    } else {
        m_sounder.muteSoundNotifications(checked);
        m_sounder.playSound("soundOn.mp3");
        ui->pushButton_sound->setIcon(QIcon(":/svg/volume_up.svg"));
    }
}

void MainWindow::on_pushButton_update_power_states_clicked() {
    if (m_state != CONTROLLER_STATES::WAIT_COMMAND) {
        showWaitFinishWarning();
        return;
    }
    ui->pushButton_update_power_states->setEnabled(false);
    retest_all_powers();
}

void MainWindow::on_pushButton_update_clicked() {
    if (m_state != CONTROLLER_STATES::WAIT_COMMAND) {
        showWaitFinishWarning();
        return;
    }
    ui->pushButton_update->setEnabled(false);
    retest_all_powers();
}

// Обработчик нажатия на кнопку выключения лампы OFF
void MainWindow::on_pushButton_switchOffOneLamp_clicked() {
    if (m_state != CONTROLLER_STATES::WAIT_COMMAND) {
        showWaitFinishWarning();
        return;
    }

    if (ui->comboBox_mode->currentIndex() == 0) {
        m_sounder.playSound("run_all_lamps_to_off_state.mp3");
        m_state = CONTROLLER_STATES::ALL_LAMPS_SWITCH_OFF_PROCESS;
        ui->label_TitlePage->setText(tlc::kStateMachineAllLampsOffCommandState);
        emit make_all_lamps_off();
        return;
    }

    qInfo() << tlc::kOperationSwitchOffOneLampName;

    if (m_powerManager->isPowerOutConnected(m_current_lamp_index)) {
        if (m_bulbs_graphics_item->setBulbOff(m_current_lamp_index)) {
            m_sounder.playSound("lamp_is_already_off.mp3");
        } else {
            m_sounder.playSound("switchOffLamp.mp3");
            m_state = CONTROLLER_STATES::ONE_LAMP_SWITCH_OFF_PROCESS;
            ui->label_TitlePage->setText(
                QString(tlc::kStateMachineOneLampOffCommandState)
                    .arg(m_current_lamp_index + 1));
            emit make_one_lamp_off(m_current_lamp_index);
            return;
        };

    } else {
        operation_failed_voice_notification();
        auto power_num = global::get_power_num_by_index(m_current_lamp_index);
        qWarning()
            << QString(tlc::kOperationSwitchOffOneLampFailed).arg(power_num);
    }
}

// Обработчик нажатия на кнопку включения лампы ON
void MainWindow::on_pushButton_switch_on_one_lamp_clicked() {
    if (m_state != CONTROLLER_STATES::WAIT_COMMAND) {
        showWaitFinishWarning();
        return;
    }
    if (ui->comboBox_mode->currentIndex() == 0) {
        m_sounder.playSound("run_all_lamps_to_on_state.mp3");
        m_state = CONTROLLER_STATES::ALL_LAMPS_SWITCH_ON_PROCESS;
        ui->label_TitlePage->setText(tlc::kStateMachineAllLampsOnCommandState);
        emit make_all_lamps_on();
        return;
    }

    if (m_powerManager->isPowerOutConnected(m_current_lamp_index)) {
        if (m_bulbs_graphics_item->setBulbOn(m_current_lamp_index)) {
            m_sounder.playSound("lamp_is_already_on.mp3");

        } else {
            m_sounder.playSound("switchOnOneLamp.mp3");
            m_state = CONTROLLER_STATES::ONE_LAMP_SWITCH_ON_PROCESS;
            emit make_one_lamp_on(m_current_lamp_index);
            ui->label_TitlePage->setText(
                QString(tlc::kStateMachineOneLampOnCommandState)
                    .arg(m_current_lamp_index + 1));
            qInfo() << tlc::kOperationSwitchOnOneLampName;
            return;
        };

    } else {
        operation_failed_voice_notification();
        auto power_num = global::get_power_num_by_index(m_current_lamp_index);
        qWarning()
            << QString(tlc::kOperationSwitchOnOneLampFailed).arg(power_num);
    }
}

void MainWindow::update_lamp_state(int lamp_index, double voltage,
                                   double current) {
    switch (m_state) {
        case CONTROLLER_STATES::WAIT_COMMAND:
            return;
            break;
        case CONTROLLER_STATES::ONE_LAMP_SWITCH_OFF_PROCESS:
            if (m_current_lamp_index > MIN_CURRENT_LAMP_INDEX) {
                if (ui->checkBox_auto_up_down->isChecked()) {
                    --m_current_lamp_index;
                }
            }
            ui->label_TitlePage->setText(tlc::kStateMachineWaitCommandState);
            m_state = CONTROLLER_STATES::WAIT_COMMAND;
            break;
        case CONTROLLER_STATES::ONE_LAMP_SWITCH_ON_PROCESS:
            if (m_current_lamp_index < MAX_CURRENT_LAMP_INDEX) {
                if (ui->checkBox_auto_up_down->isChecked()) {
                    ++m_current_lamp_index;
                }
            }
            ui->label_TitlePage->setText(tlc::kStateMachineWaitCommandState);
            m_state = CONTROLLER_STATES::WAIT_COMMAND;
            break;
        case CONTROLLER_STATES::ALL_LAMPS_SWITCH_OFF_PROCESS:
            m_bulbs_graphics_item->setBulbOff(lamp_index);
            m_current_lamp_index = lamp_index;
            if (m_current_lamp_index == MIN_CURRENT_LAMP_INDEX) {
                ui->label_TitlePage->setText(
                    tlc::kStateMachineWaitCommandState);
                m_state = CONTROLLER_STATES::WAIT_COMMAND;
            }
            break;
        case CONTROLLER_STATES::ALL_LAMPS_SWITCH_ON_PROCESS:
            m_bulbs_graphics_item->setBulbOn(lamp_index);
            m_current_lamp_index = lamp_index;
            if (m_current_lamp_index == MAX_CURRENT_LAMP_INDEX) {
                ui->label_TitlePage->setText(
                    tlc::kStateMachineWaitCommandState);
                m_state = CONTROLLER_STATES::WAIT_COMMAND;
            }
            break;
        case CONTROLLER_STATES::UPDATE_ALL_STATES_PROCESS:
            break;
    }

    m_bulbs_graphics_item->set_current_lamp_index(m_current_lamp_index);
    update_ps(global::get_power_num_by_index(lamp_index),
              global::get_power_out_by_index(lamp_index), true, voltage,
              current);
    setActivePowerOut();
    m_sceneCalibr->update();
    if (m_state == CONTROLLER_STATES::WAIT_COMMAND) {
        m_sounder.playSound("wait_for_new_command.mp3");
    }
}

void MainWindow::on_pushButton_stop_all_processes_clicked() {
    if (m_state == CONTROLLER_STATES::WAIT_COMMAND) return;

    if (m_state == CONTROLLER_STATES::ALL_LAMPS_SWITCH_OFF_PROCESS ||
        m_state == CONTROLLER_STATES::ALL_LAMPS_SWITCH_ON_PROCESS ||
        m_state == CONTROLLER_STATES::UPDATE_ALL_STATES_PROCESS) {
        m_powerManager->stopFlagForAll_Lamps.store(true);
        return;
    }
    if (m_state == CONTROLLER_STATES::ONE_LAMP_SWITCH_OFF_PROCESS ||
        m_state == CONTROLLER_STATES::ONE_LAMP_SWITCH_ON_PROCESS) {
        m_powerManager->stopFlagForOne_Lamp.store(true);
    }
}
