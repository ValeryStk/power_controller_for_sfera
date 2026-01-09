#include "MainWindow.h"
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
          if(isMuted)isMuted = false;
          else isMuted = true;
          m_sounder.muteSoundNotifications(isMuted);
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
    if(m_lampsCounter == 0) return;
    m_sounder.playSound("switchOffLamp.mp3");
    --m_lampsCounter;
    m_powerManager->decreaseVoltageStepByStepToZero(m_lampsCounter);
    ot->setBulbOff(m_lampsCounter);
    m_sceneCalibr->update();
}

void MainWindow::timeOutCaseHandler()
{
    ui->pushButton_switchOffOneLamp->setEnabled(true);
    qDebug()<<"TimeOut handler....";
}

void MainWindow::on_pushButton_switch_on_one_lamp_clicked()
{
    if(m_lampsCounter == 6)return;
    m_sounder.playSound("switchOnOneLamp.mp3");
    ot->setBulbOn(m_lampsCounter);
    //m_powerManager->increaseVoltageStepByStepToCurrentLimit(m_lampsCounter);
    m_sceneCalibr->update();
    ++m_lampsCounter;
}

