#include "MainWindow.h"
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

constexpr char kSwitchOnAllLampsText[] = "Включить все лампы";
constexpr char kSwitchOnOneLampText[] = "Включить одну лампу";
constexpr char kSwitchOffLampsText[] = "Выключить все лампы";
constexpr char kSwitchOffOneLampText[] = "Выключить одну лампу";


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createObjects();
    initializeVariables();
    setUpGui();
    makeConnects();
    setUpScene();
    qApp->installEventFilter(this);
    m_sounder.playSound("safety.mp3");
    connect(&m_afterLampsOnHeatingTimer,SIGNAL(timeout()),SLOT(showElapsedHeatingTime()));

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        m_sceneCalibr->update();
    });
    timer->start(1000);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    isStop = true;
    event->accept();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress) {
        QString objectName = obj->objectName();
        if(objectName=="labelLamp1"){return false;}
        if(objectName=="labelLamp2"){return false;}
        if(objectName=="labelLamp3"){return false;}
        if(objectName=="labelLamp4"){return false;}
        if(objectName=="labelLamp5"){return false;}
        if(objectName=="labelLamp6"){return false;}
        return false;
    } else if(event->type() == QEvent::MouseButtonDblClick){
        if(obj->objectName()=="label_TitlePage"){
            bool isMuted = m_sounder.isNotificationsMuted();
            isMuted = !isMuted;
            if(isMuted){
                m_sounder.playSound("audioNotificationsOff.mp3");
                m_sounder.muteSoundNotifications(isMuted);
            }else{
                m_sounder.muteSoundNotifications(isMuted);
                m_sounder.playSound("soundOn.mp3");
            };

        }

    }
    return false;
}


void MainWindow::showMessageBox(QMessageBox::Icon ico, QString titleText, QString text)
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

QJsonDocument MainWindow::readJsonDocumentFromFile(const QString &docName)
{
    QFile file(docName);
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qDebug() << "Ошибка разбора JSON!";
    }
    file.close();
    return doc;
}

void MainWindow::switch_on_all_lamps()
{

    auto pwrs = m_powerManager->get_power_states();
    auto lamps = pwrs["lamps"].toArray();

    for(int i=0;i<lamps.size();++i){
        //m_powerManager->increaseVoltageStepByStepToCurrentLimit(i);
        ot->set_current_lamp_index(i);
        ot->setBulbOn(i);
        m_sceneCalibr->update();
        QApplication::processEvents();
        Sleep(500);
    }

    /*for(int i=0;i<lamps.size();++i){
        auto current = m_powerManager->getCurrentValue(i);
        auto current_limit = m_powerManager->getCurrentLimit(i);
        auto diff = current_limit - current;
    }*/


}

void MainWindow::switch_off_all_lamps()
{
    auto pwrs = m_powerManager->get_power_states();
    auto lamps = pwrs["lamps"].toArray();

    for(int i=lamps.size()-1;i>=0;--i){
        m_powerManager->decreaseVoltageStepByStepToZero(i);
        ot->set_current_lamp_index(i);
        ot->setBulbOff(i);
        m_sceneCalibr->update();
        QApplication::processEvents();
        Sleep(500);
    }
    for(int i=0;i<lamps.size();++i){
        auto voltage = m_powerManager->getVoltage(i);
    }

}

void MainWindow::openRootFolder()
{
    QString rootDir = QDir::currentPath()+"/KAMEYA";
    QDir dir(rootDir);
    if(!dir.exists())dir.mkdir(rootDir);
    QString openExplorer = "c:/windows/explorer.exe /n,";
    QString path= QDir::toNativeSeparators(QDir::currentPath()+"/KAMEYA");
    openExplorer.append(path);
    QStringList args;
    QProcess::startDetached(openExplorer, args);
}

void MainWindow::testSlot()
{

    bool first_power_state =  m_powerManager->getPowerStatus(0);
    bool second_power_sate = m_powerManager->getPowerStatus(2);
    bool third_power_state = m_powerManager->getPowerStatus(4);

    ot->set_bulb_states({bulb_state::OFF,bulb_state::OFF,bulb_state::OFF,bulb_state::OFF,bulb_state::OFF,bulb_state::OFF});
    m_sceneCalibr->update();

    QVector<QPair<QLabel*,bool>> labels = {
        {ui->label_test_power_1, first_power_state},
        {ui->label_test_power_2, second_power_sate},
        {ui->label_test_power_3, third_power_state}
    };
    bool testOk = false;
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
    }else{
        m_sounder.playSound("network_error.mp3");
    }

    ui->pushButton_Forward->setEnabled(true);
}


void MainWindow::initializeVariables()
{

    QStringList pages = {
        "Техника безопасности",
        "Тестирование",
        "Калибровка"
    };
    for(int i=0;i<pages.size();++i) m_pages.insert(i,pages.at(i));

    m_isTestPassed = false;
    m_isControlPressed = false;
    m_lampsCounter = 6;

}

void MainWindow::createObjects()
{
    m_powerManager = new PowerSupplyManager;
    m_sceneCalibr = new QGraphicsScene;
    repeatLastNotification = new QShortcut(this);
    repeatLastNotification->setKey(Qt::CTRL + Qt::Key_R);
    QObject::connect(repeatLastNotification,SIGNAL(activated()),&m_sounder,SLOT(playLastSound()));
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
    ui->pushButton_switch_on_one_lamp->setIcon(QIcon(":/guiPictures/trending_up.svg"));
    ui->pushButton_switch_on_one_lamp->setIconSize(QSize(64,64));
    ui->pushButton_switchOffOneLamp->setText(kSwitchOffLampsText);
    ui->pushButton_switch_on_one_lamp->setText(kSwitchOnAllLampsText);
    ui->pushButton_sound->setIcon(QIcon(":/guiPictures/volume_up.svg"));
    ui->pushButton_sound->setIconSize(QSize(64,64));
}

void MainWindow::setUpScene()
{
    ui->graphicsView->setScene(m_sceneCalibr);
    m_sceneCalibr->setSceneRect(0, 0, 800, 600);
    ot = new OpticTable;
    ot->setZValue(1000);
    m_sceneCalibr->addItem(ot);
    m_sceneCalibr->update();
}

void MainWindow::makeConnects()
{

}

void MainWindow::openFolderInExplorer()
{
    QString openExplorer = "c:/windows/explorer.exe";
    QStringList args;
    args.append(QDir::toNativeSeparators(QDir::currentPath()));
    QProcess::startDetached(openExplorer, args);
}

void MainWindow::afterLampWasSwitchedOff()
{
    --m_lampsCounter;
    qDebug()<<"After one lamp was switched off point: "<<m_lampsCounter;
    ot->setBulbOff(m_lampsCounter);
    m_sceneCalibr->update();
    if(m_lampsCounter == 0){
        qDebug()<<"Calibration process was finished...";
        m_sounder.playSound("calibrationFinish.mp3");
    }
    else {
        m_sounder.playSound("oneLampWasSwitchedOff.mp3");
        ui->pushButton_switchOffOneLamp->setEnabled(true);
    }
}

void MainWindow::showElapsedHeatingTime()
{
    static long long time = 0;
    ++time;
    //ui->label_afterHeatingTime->setText(QDateTime::fromSecsSinceEpoch(time).toUTC().toString("hh:mm:ss"));

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

void MainWindow::on_pushButton_switchOffOneLamp_clicked()
{

    if(ui->comboBox__mode->currentIndex()==0){
        m_sounder.playSound("run_all_lamps_to_off_state.mp3");
        QTimer::singleShot(1000,this,&MainWindow::switch_off_all_lamps);
        return;
    }

    if(ot->setBulbOff(m_lampsCounter)){
        m_sounder.playSound("lamp_is_already_off.mp3");

    }else{
        m_sounder.playSound("switchOffLamp.mp3");
        m_powerManager->decreaseVoltageStepByStepToZero(m_lampsCounter);
    };
    if(m_lampsCounter > 0){
        if(ui->comboBox__mode->currentIndex()==0){
            --m_lampsCounter;
        }
    }
    ot->set_current_lamp_index(m_lampsCounter);
    m_sceneCalibr->update();
}

void MainWindow::timeOutCaseHandler()
{
    ui->pushButton_switchOffOneLamp->setEnabled(true);
    qDebug()<<"TimeOut handler....";
}

void MainWindow::on_pushButton_switch_on_one_lamp_clicked()
{

    if(ui->comboBox__mode->currentIndex()==0){
        m_sounder.playSound("run_all_lamps_to_on_state.mp3");
        QTimer::singleShot(1000,this,&MainWindow::switch_on_all_lamps);
        return;
    }

    if(ot->setBulbOn(m_lampsCounter)){
        m_sounder.playSound("lamp_is_already_on.mp3");

    }else{
        m_sounder.playSound("switchOnOneLamp.mp3");
        //m_powerManager->increaseVoltageStepByStepToCurrentLimit(m_lampsCounter);
    };

    if(m_lampsCounter < 5){
        if(ui->comboBox__mode->currentIndex()==0){
            ++m_lampsCounter;
        }
    }
    ot->set_current_lamp_index(m_lampsCounter);
    m_sceneCalibr->update();
}


void MainWindow::on_comboBox__mode_currentIndexChanged(int index)
{
    static bool is_first_start_index = true;
    if(is_first_start_index){
        m_lampsCounter = 5;
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
    m_lampsCounter = index - 1;

    ot->set_current_lamp_index(m_lampsCounter);
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

