#include "MainWindow.h"

#include "graphics_items/power_supply_item.h"
#include "qgraphicsproxywidget.h"
#include "ui_MainWindow.h"
#include "Version.h"
#include <QProcess>
#include <QDir>
#include <QTimer>
#include <QStyle>
#include "math.h"
#include <QGraphicsScene>
#include "graphics_items/OpticTable.h"
#include <chrono>
#include <QPair>
#include <memory>
#include "QStyleFactory"
#include <QDateTime>
#include "Windows.h"
#include "config.h"
#include "text_log_constants.h"


constexpr int NUMBER_OF_LAMPS = 6;
constexpr int MAX_CURRENT_LAMP_INDEX = 5;
constexpr int MIN_CURRENT_LAMP_INDEX = 0;

constexpr char kSwitchOnAllLampsText[] = "Включить все лампы";
constexpr char kSwitchOnOneLampText[] = "Включить одну лампу";
constexpr char kSwitchOffLampsText[] = "Выключить все лампы";
constexpr char kSwitchOffOneLampText[] = "Выключить одну лампу";


constexpr int lamp_pwr_out[NUMBER_OF_LAMPS][2] = {{1,1},
                                                  {1,2},
                                                  {2,1},
                                                  {2,2},
                                                  {3,1},
                                                  {3,2}};

namespace{
int get_power_num_by_index(int index){
    if(index<0||index>=NUMBER_OF_LAMPS)return-1;
    return lamp_pwr_out[index][0];
}
int get_power_out_by_index(int index){
    if(index<0||index>=NUMBER_OF_LAMPS)return-1;
    return lamp_pwr_out[index][1];
}
} //namespace

PowerSupplyItem* psi1;
PowerSupplyItem* psi2;
PowerSupplyItem* psi3;

QTimer *m_timer_to_update_power_states;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createObjects();
    initializeVariables();
    setUpGui();
    setUpScene();
    qApp->installEventFilter(this);
    m_sounder.playSound("safety.mp3");

    m_timer_to_update_power_states = new QTimer(this);
    m_timer_to_update_power_states->setInterval(1000);

    connect(m_timer_to_update_power_states, &QTimer::timeout, this, [this]() {

        for(int i=0;i<NUMBER_OF_LAMPS;++i){
            bool isConnected = m_powerManager->isPowerOutConnected(i);
            PowerUnitParams result{false,0.000,0.000,0.000};
            if(isConnected){
                result = m_powerManager->get_all_params_for_lamp_out(i);
            }
            int power_num = lamp_pwr_out[i][0];
            int out_num = lamp_pwr_out[i][1];
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
    m_powerManager->deleteLater();
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
            ui->checkBox_5_cooling_for_big_sphere->isChecked())return true;
    else return false;
}

void MainWindow::operation_failed()
{
    QTimer::singleShot(4000,[this](){m_sounder.playSound("operation_failed.mp3");});
}

void MainWindow::switch_on_all_lamps()
{

    qInfo()<<tlc::kOperatinAllLampsSwitchOnName;
    auto pwrs = m_powerManager->get_power_states();
    auto lamps = pwrs["lamps"].toArray();

    for(int i=0;i<lamps.size();++i){
        m_current_lamp_index = i;
        ot->set_current_lamp_index(i);
        if(m_powerManager->isPowerOutConnected(m_current_lamp_index)){
            m_powerManager->increaseVoltageStepByStepToCurrentLimit(i);
            ot->setBulbOn(i);
        }else{
            auto power_num = get_power_num_by_index(i);
            qWarning()<<QString(tlc::kOperationAllLampsSwitchOnFailed).arg(power_num);
            operation_failed();
            break;
        }
        m_sceneCalibr->update();
        QApplication::processEvents();
        Sleep(200);
    }
}

void MainWindow::switch_off_all_lamps()
{
    qInfo()<<tlc::kOperatinAllLampsSwitchOffName;
    auto pwrs = m_powerManager->get_power_states();
    auto lamps = pwrs["lamps"].toArray();

    for(int i=lamps.size()-1;i>=0;--i){
        m_current_lamp_index = i;
        ot->set_current_lamp_index(i);
        if(m_powerManager->isPowerOutConnected(m_current_lamp_index)){
            m_powerManager->decreaseVoltageStepByStepToZero(i);
            ot->setBulbOff(i);
        }else{
            auto power_num = get_power_num_by_index(i);
            qWarning()<<QString(tlc::kOperationAllLampsSwitchOffFailed).arg(power_num);
            operation_failed();
            break;
        }
        m_sceneCalibr->update();
        QApplication::processEvents();
        Sleep(200);
    }
}

void MainWindow::openRootFolder()
{
    QString rootDir = QApplication::applicationDirPath() + global::path_to_log_dir;
    QDir().mkpath(rootDir);
    QString explorer = "C:/Windows/explorer.exe";
    QStringList args;
    args << QDir::toNativeSeparators(rootDir);
    QProcess::startDetached(explorer, args);
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
    int current_pwr_num = get_power_num_by_index(m_current_lamp_index);
    int active_out = get_power_out_by_index(m_current_lamp_index);
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
    bool second_power_sate = m_powerManager->getPowerStatus(2);
    bool third_power_state = m_powerManager->getPowerStatus(4);

    auto pwrs = m_powerManager->get_power_states();
    QJsonArray lamps = pwrs["lamps"].toArray();
    QVector<bulb_state> bulbs_states(lamps.size());

    for(int i=0; i<lamps.size(); ++i){
        auto current_limit_value = lamps[i].toObject().value("max_current").toDouble();
        auto current_present_value = m_powerManager->getCurrentValue(i);
        auto current_voltage = m_powerManager->getVoltage(i);
        if(current_voltage == 0 || current_voltage <= 0.05){
            bulbs_states[i] = bulb_state::OFF;
        }else if(qAbs(current_limit_value - current_present_value) < 0.03){
            bulbs_states[i] = bulb_state::ON;
        }else{
            bulbs_states[i] = bulb_state::UNDEFINED;
            m_sounder.playSound("notLoaded.mp3");
        }
    }

    ot->set_bulb_states(bulbs_states);
    m_sceneCalibr->update();

    QVector<QPair<QLabel*,bool>> labels = {
        {ui->label_test_power_1, first_power_state},
        {ui->label_test_power_2, second_power_sate},
        {ui->label_test_power_3, third_power_state}
    };
    bool testOk = false;
    if(first_power_state && second_power_sate && third_power_state){
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
        qInfo()<<"Start test is OK!";
    }else{
        m_sounder.playSound("network_error.mp3");
        if(pwrs["is_unlock"].toBool()){
            ui->pushButton_Forward->setEnabled(true);
            qWarning()<<"Start test is failed :(";
            qWarning()<<"Forward button was unclocked. ----->";
        }
    }

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
    auto pwrs = m_powerManager->get_power_states().value("lamps").toArray();
    for(int i=0;i<pwrs.size();++i){
        ui->comboBox__mode->addItem(pwrs[i].toObject().value("name").toString());
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
}

void MainWindow::setUpScene()
{
    ui->graphicsView->setScene(m_sceneCalibr);
    QVector<QColor> bulb_colors;
    bulb_colors.resize(6);
    QJsonObject jo;
    jo = m_powerManager->get_power_states();
    auto lamps = jo["lamps"].toArray();
    for(int i=0;i<lamps.size();++i){
        bulb_colors[i] = QColor(lamps[i].toObject()["color"].toString());
    }
    m_sceneCalibr->setSceneRect(0, 0, 800, 600);
    ot = new OpticTable;
    ot->setZValue(1000);
    m_sceneCalibr->addItem(ot);
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
        }
        if(m_pages.value(ui->stackedWidget->currentIndex())=="Тестирование"){
            m_sounder.playSound("startTest.mp3");
            ui->pushButton_Forward->setEnabled(false);
            QTimer::singleShot(5000,this,SLOT(testSlot()));
        }
        if(m_pages.value(ui->stackedWidget->currentIndex())=="Калибровка"){
            m_sounder.playSound("startCalibration.mp3");
            isEnd = true;
            m_timer_to_update_power_states->start();
            ui->pushButton_Forward->setVisible(false);
        }
    }
}


void MainWindow::on_checkBox_5_cooling_for_big_sphere_stateChanged(int arg1)
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

    ot->set_current_lamp_index(m_current_lamp_index);
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
    qInfo()<<tlc::kOperationUpdateAllPowersStates;
    QTimer::singleShot(5000,this,SLOT(testSlot()));
}

void MainWindow::on_pushButton_update_clicked()
{
    m_timer_to_update_power_states->stop();
    testSlot();
    m_timer_to_update_power_states->start();
}

// Обраблтчик нажатия на кнопку выключения OFF
void MainWindow::on_pushButton_switchOffOneLamp_clicked()
{

    if(ui->comboBox__mode->currentIndex()==0){
        m_sounder.playSound("run_all_lamps_to_off_state.mp3");
        QTimer::singleShot(1000,this,&MainWindow::switch_off_all_lamps);
        return;
    }

    qInfo()<<tlc::kOperationSwitchOffOneLampName;
    if(m_powerManager->isPowerOutConnected(m_current_lamp_index)){

        if(ot->setBulbOff(m_current_lamp_index)){
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
        ot->set_current_lamp_index(m_current_lamp_index);
        m_sceneCalibr->update();

    }else{
        operation_failed();
        auto power_num = get_power_num_by_index(m_current_lamp_index);
        qWarning()<<QString(tlc::kOperationSwitchOffOneLampFailed).arg(power_num);
    }
}

// Обраблтчик нажатия на кнопку включения ON
void MainWindow::on_pushButton_switch_on_one_lamp_clicked()
{

    if(ui->comboBox__mode->currentIndex()==0){
        m_sounder.playSound("run_all_lamps_to_on_state.mp3");
        QTimer::singleShot(1000,this,&MainWindow::switch_on_all_lamps);
        return;
    }

    qInfo()<<tlc::kOperationSwitchOnOneLampName;
    if(m_powerManager->isPowerOutConnected(m_current_lamp_index)){

        if(ot->setBulbOn(m_current_lamp_index)){
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
        ot->set_current_lamp_index(m_current_lamp_index);
        m_sceneCalibr->update();

    }else{
        operation_failed();
        auto power_num = get_power_num_by_index(m_current_lamp_index);
        qWarning()<<QString(tlc::kOperationSwitchOnOneLampFailed).arg(power_num);
    }
}
