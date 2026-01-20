#include "MainWindow.h"
#include "qmutex.h"
#include "ui_MainWindow.h"

#include "Version.h"
#include <QProcess>
#include <QDir>
#include <QTimer>
#include <QStyle>
#include "math.h"
#include <QGraphicsScene>
#include <chrono>
#include <QPair>
#include <memory>
#include "QStyleFactory"
#include <QDateTime>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <synchapi.h>
#include "config.h"
#include "text_log_constants.h"
#include <QDesktopServices>
#include <QUrl>
#include "icon_generator.h"
#include <QtConcurrent/QtConcurrent>


constexpr char kSwitchOnAllLampsText[] = "Включить все лампы";
constexpr char kSwitchOnOneLampText[]  = "Включить одну лампу";
constexpr char kSwitchOffLampsText[]   = "Выключить все лампы";
constexpr char kSwitchOffOneLampText[] = "Выключить одну лампу";
lamps_powers_config cfg;

namespace{

void openFileByDefaultSoftware(const QString &filePath) {
    QUrl url = QUrl::fromLocalFile(filePath);
    QDesktopServices::openUrl(url); }


void append_v_i_to_log(const QString& filePath,
                       const QString& line){
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

} //namespace


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    global::mayBe_create_log_dir();
    global::get_config_struct(global::json_lamps_file_name,cfg);
    createObjects();
    initializeVariables();
    setUpGui();
    setUpScene();
    qApp->installEventFilter(this);
    m_sounder.muteSoundNotifications(!cfg.is_sound);
    ui->pushButton_sound->setChecked(!cfg.is_sound);
    m_sounder.playSound("safety.mp3");

    m_timer_to_update_power_states = new QTimer(this);
    m_timer_to_update_power_states->setInterval(1000);

    connect(m_timer_to_update_power_states, &QTimer::timeout, this, [this]() {

        for(int i=0;i<NUMBER_OF_LAMPS;++i){
            bool isConnected = m_powerManager->isPowerOutConnected(i);
            PowerUnitParams result{false,0.000,0.000,0.000};
            int power_num = global::get_power_num_by_index(i);
            int out_num = global::get_power_out_by_index(i);
            if(isConnected){
                result = m_powerManager->get_all_params_for_lamp_out(i);
                QString v_i_line = QString("%1 %2 %3 %4").arg(i+1)
                        .arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh:mm:ss"))
                        .arg(result.V)
                        .arg(result.I);
                QString full_path_to_cv_log = QApplication::applicationDirPath()+
                        global::relative_path_to_cv_log_file;
                append_v_i_to_log(full_path_to_cv_log,v_i_line);
            }
            update_ps(power_num, out_num, result.isOn, result.V, result.I);
        }
        m_sceneCalibr->update();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    m_sceneCalibr->removeItem(psi1);
    delete psi1;
    m_sceneCalibr->removeItem(psi2);
    delete psi2;
    m_sceneCalibr->removeItem(psi3);
    delete psi3;
    m_sceneCalibr->removeItem(m_bulbs_graphics_item);
    delete m_bulbs_graphics_item;
    m_powerManager->deleteLater();
    QApplication::processEvents();
    event->accept();
}


void MainWindow::showMessageBox(QMessageBox::Icon ico,
                                QString titleText,
                                QString text)
{
    if(text.isEmpty())text = "Нет данных";
    QMessageBox msgBox;
    msgBox.setIcon(ico);
    msgBox.setText(titleText);
    msgBox.setInformativeText(text);
    msgBox.exec();
}


bool MainWindow::checkSafetyUser()
{
    if(ui->checkBox_1_uv->isChecked()&&
            ui->checkBox_2_power->isChecked()&&
            ui->checkBox_cooling_for_big_sphere->isChecked())return true;
    else return false;
}

void MainWindow::operation_failed_voice_notification()
{

    QTimer::singleShot(4000,this,[this](){
        m_sounder.playSound("operation_failed.mp3");});
    showMessageBox(QMessageBox::Warning,"Ошибка","Операция провалилась, смотрите logic.log");
}

void MainWindow::retest_all_powers()
{
    qInfo()<<tlc::kOperationUpdateAllPowersStates;
    m_timer_to_update_power_states->stop();
    QTimer::singleShot(5000,this,SLOT(testSlot()));
}

void MainWindow::switch_on_all_lamps()
{
    m_timer_to_update_power_states->stop();
    qInfo()<<tlc::kOperatinAllLampsSwitchOnName;
    auto pwrs = m_powerManager->get_power_states();

    for(int i=0;i < NUMBER_OF_LAMPS; ++i){
        m_current_lamp_index = i;
        m_bulbs_graphics_item->set_current_lamp_index(i);
        if(m_powerManager->isPowerOutConnected(m_current_lamp_index)){
            m_powerManager->increaseVoltageStepByStepToCurrentLimit(i);
            auto current = m_powerManager->getCurrentValue(i);
            if(qAbs(cfg.lamps_array[i].max_current - current)<global::kCurrentTargetAccuracy){
            m_bulbs_graphics_item->setBulbOn(i);
            }else{
            m_bulbs_graphics_item->setBulbUndefined(i);
            }
        }else{
            auto power_num = global::get_power_num_by_index(i);
            qWarning()<<QString(tlc::kOperationAllLampsSwitchOnFailed).arg(power_num);
            operation_failed_voice_notification();
            m_bulbs_graphics_item->setBulbUndefined(i);
            break;
        }
        m_sceneCalibr->update();
        QApplication::processEvents();
    }
    m_sceneCalibr->update();
    m_timer_to_update_power_states->start();
}

void MainWindow::switch_off_all_lamps()
{
    m_timer_to_update_power_states->stop();
    qInfo()<<tlc::kOperatinAllLampsSwitchOffName;
    auto pwrs = m_powerManager->get_power_states();
    auto lamps = pwrs[global::kJsonKeyLampsArray].toArray();

    for(int i=lamps.size()-1; i>=0; --i){
        m_current_lamp_index = i;
        m_bulbs_graphics_item->set_current_lamp_index(i);
        if(m_powerManager->isPowerOutConnected(m_current_lamp_index)){
            m_powerManager->decreaseVoltageStepByStepToZero(i);
            m_bulbs_graphics_item->setBulbOff(i);
        }else{
            auto power_num = global::get_power_num_by_index(i);
            qWarning()<<QString(tlc::kOperationAllLampsSwitchOffFailed).arg(power_num);
            operation_failed_voice_notification();
            break;
        }
        m_sceneCalibr->update();
        QApplication::processEvents();
    }
    m_timer_to_update_power_states->start();
}

void MainWindow::update_ps(int ps,
                           int out,
                           bool isOn,
                           double voltage,
                           double current)
{
    PowerSupplyItem* ps_item = nullptr;
    psi1->set_all_outs_unactive();
    psi2->set_all_outs_unactive();
    psi3->set_all_outs_unactive();

    switch (ps) {
    case 1:
        ps_item = psi1;
        break;
    case 2:
        ps_item = psi2;
        break;
    case 3:
        ps_item = psi3;
        break;
    default:return;
    }

    if(ps_item == nullptr)return;


    if(out == 1){
        ps_item->set_voltage_out_1(voltage);
        ps_item->set_current_out_1(current);
        ps_item->set_enabled_out_1(isOn);
    }

    if(out == 2){
        ps_item->set_voltage_out_2(voltage);
        ps_item->set_current_out_2(current);
        ps_item->set_enabled_out_2(isOn);
    }

    //m_current_lamp_index
    int current_pwr_num = global::get_power_num_by_index(m_current_lamp_index);
    int active_out = global::get_power_out_by_index(m_current_lamp_index);
    if(current_pwr_num == 1){
        ps_item = psi1;
    }else if(current_pwr_num == 2){
        ps_item = psi2;
    }else if (current_pwr_num == 3) {
        ps_item = psi3;
    }
    if(active_out == 1){
        ps_item->set_out_1_active();
    }
    if(active_out == 2){
        ps_item->set_out_2_active();
    }
}

void MainWindow::testSlot()
{
    bool first_power_state =  m_powerManager->getPowerStatus(0);
    bool second_power_state = m_powerManager->getPowerStatus(2);
    bool third_power_state =  m_powerManager->getPowerStatus(4);

    bulb_state bulbs_states[NUMBER_OF_LAMPS];
    bool power_states[NUMBER_OF_POWER_SUPPLIES] = {first_power_state,
                                                  second_power_state,
                                                  third_power_state};
    for(int i=0; i < NUMBER_OF_LAMPS; ++i){
        auto json_current_limit_value = cfg.lamps_array[i].max_current;
        auto current_max_value = m_powerManager->getCurrentLimit(i);
        auto current_present_value = m_powerManager->getCurrentValue(i);
        auto current_voltage = m_powerManager->getVoltage(i);
        auto power_num = global::get_power_num_by_index(i);
        bool is_connected = power_states[power_num-1];
        qDebug()<<"is_connected: "<<i<<is_connected<<power_num;
        if((current_voltage <= global::kVoltageZeroAccuracy) && is_connected){
            bulbs_states[i] = bulb_state::OFF;
        }else if(qAbs(json_current_limit_value - current_present_value) < global::kCurrentTargetAccuracy){
            bulbs_states[i] = bulb_state::ON;
        }else{
            bulbs_states[i] = bulb_state::UNDEFINED;
        }
        qDebug()<<"current limit: "<<i<<current_max_value;
        if(i==0)psi1->set_max_current_out_1(current_max_value);
        if(i==1)psi1->set_max_current_out_2(current_max_value);
        if(i==2)psi2->set_max_current_out_1(current_max_value);
        if(i==3)psi2->set_max_current_out_2(current_max_value);
        if(i==4)psi3->set_max_current_out_1(current_max_value);
        if(i==5)psi3->set_max_current_out_2(current_max_value);
    }

    m_bulbs_graphics_item->set_bulb_states(bulbs_states);
    m_sceneCalibr->update();

    QVector<QPair<QLabel*,bool>> labels = {
        {ui->label_test_power_1, first_power_state},
        {ui->label_test_power_2, second_power_state},
        {ui->label_test_power_3, third_power_state}
    };
    bool testOk = false;
    if(first_power_state && second_power_state && third_power_state){
        testOk = true;
    }
    for(int i=0;i<labels.size();++i){

        QString style = "";
        if(labels[i].second)style = "background:rgb(0,128,0)";
        else {
            style = "background:rgb(128,0,0)";
            testOk = false;
        }
        labels[i].first->setStyleSheet(style);
    }
    if(testOk){
        m_sounder.playSound("testOk.mp3");
        ui->pushButton_Forward->setEnabled(true);
        qInfo()<<"ALL POWERS TEST IS OK.";
    }else{
        m_sounder.playSound("network_error.mp3");
        ui->pushButton_Forward->setEnabled(cfg.is_unclock);
        qWarning()<<"ALL POWERS TEST IS FAILED.";
        qInfo()<<"POWER 1: "<<first_power_state;
        qInfo()<<"POWER 2: "<<second_power_state;
        qInfo()<<"POWER 3: "<<third_power_state;
        showMessageBox(QMessageBox::Warning,"Тест не пройден",
                       "Блоки питания не готовы к работе.");
    }
    m_timer_to_update_power_states->start();
    ui->pushButton_update_power_states->setEnabled(true);
    ui->pushButton_update->setEnabled(true);
}

void MainWindow::initializeVariables()
{

    QStringList pages = {
        "Техника безопасности",
        "Тестирование",
        "Калибровка"
    };
    for(int i=0;i<pages.size();++i) m_pages.insert(i,pages.at(i));

    m_current_lamp_index = MAX_CURRENT_LAMP_INDEX;

}

void MainWindow::createObjects()
{
    m_powerManager = new PowerSupplyManager;
    m_sceneCalibr = new QGraphicsScene;

    repeatLastNotification = new QShortcut(this);
    open_log_dir = new QShortcut(this);;
    show_log = new QShortcut(this);

    repeatLastNotification->setKey(Qt::CTRL + Qt::Key_R);
    open_log_dir->setKey(Qt::CTRL + Qt::Key_L);
    QObject::connect(repeatLastNotification,SIGNAL(activated()),&m_sounder,SLOT(playLastSound()));
    QObject::connect(open_log_dir,SIGNAL(activated()),this,SLOT(openRootFolder()));
    connect(ui->pushButton_repeat_last_sound,SIGNAL(clicked()),&m_sounder,SLOT(playLastSound()));
}

void MainWindow::setUpGui()
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setAttribute(Qt::AA_Use96Dpi);
    QString appTitle = "POWER CONTROLLER ";
    appTitle.append(VER_PRODUCTVERSION_STR);
    setWindowTitle(appTitle);
    ui->label_TitlePage->setText(m_pages.value(ui->stackedWidget->currentIndex()));
    ui->centralwidget->setStyleSheet("background-color:#2E2F30;color:lightgrey");
    ui->pushButton_Backward->setVisible(false);
    ui->stackedWidget->setCurrentIndex(0);
    ui->comboBox__mode->addItem("Все лампы");
    auto pwrs = m_powerManager->get_power_states().value(global::kJsonKeyLampsArray).toArray();
    for(int i=0;i<pwrs.size();++i){
        QString str_color = pwrs[i].toObject().value("color").toString();
        QColor color(str_color);
        auto icon = iut::createIcon(color.red(),color.green(),color.blue());
        ui->comboBox__mode->addItem(icon,pwrs[i].toObject().value("name").toString());

    }
    ui->pushButton_switchOffOneLamp->setIcon(QIcon(":/guiPictures/trending_down.svg"));
    ui->pushButton_switchOffOneLamp->setIconSize(QSize(64,64));
    ui->pushButton_switchOffOneLamp->setText(kSwitchOffLampsText);

    ui->pushButton_switch_on_one_lamp->setIcon(QIcon(":/guiPictures/trending_up.svg"));
    ui->pushButton_switch_on_one_lamp->setIconSize(QSize(64,64));
    ui->pushButton_switch_on_one_lamp->setText(kSwitchOnAllLampsText);

    ui->pushButton_update_power_states->setIcon(QIcon(":/guiPictures/update.svg"));
    ui->pushButton_update_power_states->setIconSize(QSize(64,64));

    ui->pushButton_update->setIcon(QIcon(":/guiPictures/update.svg"));
    ui->pushButton_update->setIconSize(QSize(64,64));

    ui->pushButton_sound->setIcon(QIcon(":/guiPictures/volume_up.svg"));
    ui->pushButton_sound->setIconSize(QSize(64,64));

    ui->pushButton_open_log->setIcon(QIcon(":/guiPictures/log.svg"));
    ui->pushButton_open_log->setIconSize(QSize(64,64));
}

void MainWindow::setUpScene()
{
    ui->graphicsView->setScene(m_sceneCalibr);
    QVector<QColor> bulb_colors;
    bulb_colors.resize(6);
    QJsonObject jo;
    jo = m_powerManager->get_power_states();
    auto lamps = jo[global::kJsonKeyLampsArray].toArray();
    for(int i=0;i<lamps.size();++i){
        bulb_colors[i] = QColor(lamps[i].toObject()["color"].toString());
    }
    m_sceneCalibr->setSceneRect(0, 0, 1000, 600);
    m_bulbs_graphics_item = new BulbsQGraphicsItem;
    m_bulbs_graphics_item->setZValue(1000);
    m_sceneCalibr->addItem(m_bulbs_graphics_item);
    m_sceneCalibr->update();
    psi1 = new PowerSupplyItem(":/guiPictures/PS.svg",jo["ps1"].toObject().value("name").toString(),"ps1");
    psi1->setScale(0.8);
    psi1->set_out_1_color(QColor(bulb_colors[0]));
    psi1->set_out_2_color(QColor(bulb_colors[1]));
    m_sceneCalibr->addItem(psi1);
    psi2 = new PowerSupplyItem(":/guiPictures/PS.svg",jo["ps2"].toObject().value("name").toString(),"ps2");
    psi2->setScale(0.8);
    psi2->set_out_1_color(QColor(bulb_colors[2]));
    psi2->set_out_2_color(QColor(bulb_colors[3]));
    m_sceneCalibr->addItem(psi2);
    psi3 = new PowerSupplyItem(":/guiPictures/PS.svg",jo["ps3"].toObject().value("name").toString(),"ps3");
    psi3->setScale(0.8);
    psi3->set_out_1_color(QColor(bulb_colors[4]));
    psi3->set_out_2_color(QColor(bulb_colors[5]));
    m_sceneCalibr->addItem(psi3);

}


// GUI handlers
void MainWindow::on_pushButton_Backward_clicked()
{
    if(ui->stackedWidget->currentIndex()>0){
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()-1);
        ui->label_TitlePage->setText(m_pages.value(ui->stackedWidget->currentIndex()));
    }
}

void MainWindow::on_pushButton_Forward_clicked()
{
    static bool isEnd = false;
    if(isEnd)return;
    if(ui->stackedWidget->currentIndex()<ui->stackedWidget->count()-1){

        if(m_pages.value(ui->stackedWidget->currentIndex())=="Техника безопасности"){

            if(!checkSafetyUser()){
                m_sounder.playSound("checkWarnings.mp3");
                return;
            }
        }

        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()+1);
        ui->label_TitlePage->setText(m_pages.value(ui->stackedWidget->currentIndex()));

        if(m_pages.value(ui->stackedWidget->currentIndex())=="Техника безопасности"){
            m_sounder.playSound("safety.mp3");
            return;
        }
        if(m_pages.value(ui->stackedWidget->currentIndex())=="Тестирование"){
            //m_sounder.playSound("startTest.mp3");
            ui->pushButton_Forward->setEnabled(false);
            QTimer::singleShot(1000,this,SLOT(testSlot()));
            return;
        }
        if(m_pages.value(ui->stackedWidget->currentIndex())=="Калибровка"){
            m_sounder.playSound("startCalibration.mp3");
            isEnd = true;
            m_timer_to_update_power_states->start();
            ui->pushButton_Forward->setVisible(false);
        }
    }
}

void MainWindow::on_pushButton_open_log_clicked()
{
    auto pathToLogicLog = QApplication::applicationDirPath()+global::relative_path_to_logic_log_file;
    auto pathToCV_Log = QApplication::applicationDirPath()+global::relative_path_to_cv_log_file; 
    openFileByDefaultSoftware(pathToCV_Log);
    openFileByDefaultSoftware(pathToLogicLog);
}


void MainWindow::on_checkBox_cooling_for_big_sphere_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        m_sounder.playSound("dontForgetCooling.mp3");
    };
}

void MainWindow::on_comboBox__mode_currentIndexChanged(int index)
{
    static bool is_first_start_index = true;
    if(is_first_start_index){
        m_current_lamp_index = MAX_CURRENT_LAMP_INDEX;
        is_first_start_index = false;
        return;
    }

    if(index==0){
        ui->pushButton_switchOffOneLamp->setText(kSwitchOffLampsText);
        ui->pushButton_switch_on_one_lamp->setText(kSwitchOnAllLampsText);
    }else{
        ui->pushButton_switchOffOneLamp->setText(kSwitchOffOneLampText);
        ui->pushButton_switch_on_one_lamp->setText(kSwitchOnOneLampText);
    }

    if(index < 1 || index > 6)return;
    m_current_lamp_index = index - 1;

    m_bulbs_graphics_item->set_current_lamp_index(m_current_lamp_index);
    m_sceneCalibr->update();
}

void MainWindow::on_pushButton_sound_toggled(bool checked)
{
    if(checked){
        m_sounder.playSound("audioNotificationsOff.mp3");
        m_sounder.muteSoundNotifications(checked);
        ui->pushButton_sound->setIcon(QIcon(":/guiPictures/no_sound.svg"));

    }else{
        m_sounder.muteSoundNotifications(checked);
        m_sounder.playSound("soundOn.mp3");
        ui->pushButton_sound->setIcon(QIcon(":/guiPictures/volume_up.svg"));
    }

}

void MainWindow::on_pushButton_update_power_states_clicked()
{
    ui->pushButton_update_power_states->setEnabled(false);
    retest_all_powers();
}

void MainWindow::on_pushButton_update_clicked()
{
    ui->pushButton_update->setEnabled(false);
    retest_all_powers();
}

// Обработчик нажатия на кнопку выключения лампы OFF
void MainWindow::on_pushButton_switchOffOneLamp_clicked()
{

    if(ui->comboBox__mode->currentIndex()==0){
        m_sounder.playSound("run_all_lamps_to_off_state.mp3");
        QTimer::singleShot(1000,this,&MainWindow::switch_off_all_lamps);
        return;
    }

    qInfo()<<tlc::kOperationSwitchOffOneLampName;
    if(m_powerManager->isPowerOutConnected(m_current_lamp_index)){

        if(m_bulbs_graphics_item->setBulbOff(m_current_lamp_index)){
            m_sounder.playSound("lamp_is_already_off.mp3");
        }else{
            m_sounder.playSound("switchOffLamp.mp3");
            m_powerManager->decreaseVoltageStepByStepToZero(m_current_lamp_index);
        };
        if(m_current_lamp_index > MIN_CURRENT_LAMP_INDEX){
            if(ui->checkBox_auto_up_down->isChecked()){
                --m_current_lamp_index;
            }
        }
        m_bulbs_graphics_item->set_current_lamp_index(m_current_lamp_index);
        m_sceneCalibr->update();

    }else{
        operation_failed_voice_notification();
        auto power_num = global::get_power_num_by_index(m_current_lamp_index);
        qWarning()<<QString(tlc::kOperationSwitchOffOneLampFailed).arg(power_num);
    }
}

// Обработчик нажатия на кнопку включения лампы ON
void MainWindow::on_pushButton_switch_on_one_lamp_clicked()
{

    if(ui->comboBox__mode->currentIndex()==0){
        m_sounder.playSound("run_all_lamps_to_on_state.mp3");
        QTimer::singleShot(1000,this,&MainWindow::switch_on_all_lamps);
        return;
    }

    qInfo()<<tlc::kOperationSwitchOnOneLampName;
    if(m_powerManager->isPowerOutConnected(m_current_lamp_index)){

        if(m_bulbs_graphics_item->setBulbOn(m_current_lamp_index)){
            m_sounder.playSound("lamp_is_already_on.mp3");

        }else{
            m_sounder.playSound("switchOnOneLamp.mp3");
            m_powerManager->increaseVoltageStepByStepToCurrentLimit(m_current_lamp_index);
        };

        if(m_current_lamp_index < MAX_CURRENT_LAMP_INDEX){
            if(ui->checkBox_auto_up_down->isChecked()){
                ++m_current_lamp_index;
            }
        }
        m_bulbs_graphics_item->set_current_lamp_index(m_current_lamp_index);
        m_sceneCalibr->update();

    }else{
        operation_failed_voice_notification();
        auto power_num = global::get_power_num_by_index(m_current_lamp_index);
        qWarning()<<QString(tlc::kOperationSwitchOnOneLampFailed).arg(power_num);
    }
}
