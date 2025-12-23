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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createObjects();
    initializeVariables();
    setUpGui();
    makeConnects();
    qApp->installEventFilter(this);
    m_sounder.playSound("welcome.mp3");
    getProcessParams();
    getExpositions();
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

void MainWindow::createTreeFolders()
{
    rootDir = QDir::currentPath()+"/KAMEYA";
    QDir dir(rootDir);
    if(!dir.exists())dir.mkdir(rootDir);
    rootDir.append("/");
    rootDir.append(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    dir.mkdir(rootDir);

    QString rootIr = rootDir;
    QString rootVisible = rootDir;

    if(m_activeSensor == INFRA_RED_SENSOR){
        rootIr.append("/").append(sensorNames.irSensor);
        dir.mkdir(rootIr);
    }

    if(m_activeSensor == VISIBLE_RANGE_SENSOR){
        rootVisible.append("/").append(sensorNames.visibleSensor);
        dir.mkdir(rootVisible);
    }


    for(int i = 1;i<7;++i){

        QString tempIr = rootIr;
        QString tempVis = rootVisible;

        if(m_activeSensor == INFRA_RED_SENSOR){
            tempIr.append("/").append(QString::number(i));
            dir.mkdir(tempIr);
            QString bS_Ir = tempIr,small_S_Ir = tempIr,Laser_Ir = tempIr;
            bS_Ir.append("/").append(srcNames.bigSphere);
            dir.mkdir(bS_Ir);
            small_S_Ir.append("/").append(srcNames.smallSphere);
            dir.mkdir(small_S_Ir);
            Laser_Ir.append("/").append(srcNames.linearLights);
            dir.mkdir(Laser_Ir);
        }
        if(m_activeSensor == VISIBLE_RANGE_SENSOR){
            tempVis.append("/").append(QString::number(i));
            dir.mkdir(tempVis);
            QString bS_Vis = tempVis,small_S_Vis = tempVis,Laser_Vis = tempVis;
            bS_Vis.append("/").append(srcNames.bigSphere);
            dir.mkdir(bS_Vis);
            small_S_Vis.append("/").append(srcNames.smallSphere);
            dir.mkdir(small_S_Vis);
            Laser_Vis.append("/").append(srcNames.linearLights);
            dir.mkdir(Laser_Vis);
        }

    }

}

bool MainWindow::checkSafetyUser()
{
    if(ui->checkBox_1_uv->isChecked()&&
            ui->checkBox_2_power->isChecked()&&
            ui->checkBox_5_cooling_for_big_sphere->isChecked())return true;
    else return false;
}


void MainWindow::startMeasuringAfterGatheringExpositions()
{
    m_logic.calibrAction = CalibrSteps::GATHERING_DATA;
    m_gettingSpecs       = SpectralGetting::CloseShutter;
    expoIndex = m_expositionsList.size() - 1;
    isStop = true;
    if(src_expositions.big_sphere > src_expositions.small_spheare)
       src_expositions.big_sphere = src_expositions.small_spheare;
    if(src_expositions.big_sphere < src_expositions.small_spheare)
       src_expositions.small_spheare = src_expositions.big_sphere;
    qDebug()<<"+++++++++++++++++ START MEASURING +++++++++++++++++++++++";
    qDebug()<<"Src expositions bs: " <<src_expositions.big_sphere;
    qDebug()<<"Src expositions ss: " <<src_expositions.small_spheare;
    qDebug()<<"Src expositions ll: " <<src_expositions.linear_lights;
    qDebug()<<"Src expositions:    " <<src_expositions.isAllExpoGood;
    m_gettingSpecs = SpectralGetting::CapturingBlacks;
    setSavingPath();
    setMetaData();
    isStop = false;
    getSingleSpectr4ActiveSensor();
}

void MainWindow::setSavingPath()
{
    QString path = rootDir;
    if(m_activeSensor == INFRA_RED_SENSOR){
        path.append("/").append(sensorNames.irSensor).append("/");
    }
    if(m_activeSensor == VISIBLE_RANGE_SENSOR){
        path.append("/").append(sensorNames.visibleSensor).append("/");
    }

    path.append(QString::number(m_lampsCounter)).append("/");

    switch(m_logic.lightSource){
    case BIG_SPHERE:path.append(srcNames.bigSphere);
        break;
    case SMALL_SPHERE:path.append(srcNames.smallSphere);
        break;
    case LINEAR_LIGHTS:path.append(srcNames.linearLights);
        break;
    case NOT_POINTED:
        break;
    }
    path.append("/");


    setMetaData();
    qDebug()<<"Saving path: "<<path;
}

void MainWindow::setMetaData()
{

    QString metaData;
    metaData = "_";
    if(m_gettingSpecs == SpectralGetting::CapturingBlacks)metaData.append("_black");
    if(m_gettingSpecs == SpectralGetting::CapturingReal)  metaData.append("_real");

}

void MainWindow::changeWave()
{
    switch(m_activeSensor){
    case VISIBLE_RANGE_SENSOR:
        ++ m_process.visibleSensor.currentWaveIndex;
        if(m_process.visibleSensor.currentWaveIndex == m_process.visibleSensor.lambdas.size()){
            m_powerManager->switchOffOneLamp();
            return;
        }//In this case we have to switch off one lamp

        break;
    case INFRA_RED_SENSOR:
        ++ m_process.infraRedSensor.currentWaveIndex;
        if(m_process.infraRedSensor.currentWaveIndex == m_process.infraRedSensor.lambdas.size()){
            m_powerManager->switchOffOneLamp();
            return;
        }//In this case we have to switch off one lamp

        break;
    case UKNOWN_SENSOR:
        break;
    }
}

double MainWindow::getExpositionFromJson()
{
    QString key = QString::number(m_lampsCounter);
    key.prepend("lamp");
    QString waveVR = QString::number(m_process.visibleSensor.lambdas.at(m_process.visibleSensor.currentWaveIndex));
    QString waveIR = QString::number(m_process.infraRedSensor.lambdas.at(m_process.infraRedSensor.currentWaveIndex));

    switch (m_activeSensor) {
    case VISIBLE_RANGE_SENSOR:
        return  expositions_for_grade_1200.find(key)->toObject().value(waveVR).toDouble();
        break;
    case INFRA_RED_SENSOR:
        return expositions_for_grade_300.find(key)->toObject().value(waveIR).toDouble();
        break;
    case UKNOWN_SENSOR:
        break;
    };
    return 0;
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

void MainWindow::getProcessParams()
{

    QJsonObject m_process_params = readJsonDocumentFromFile(QDir::currentPath()+"/calibr_process.json").object();
    QJsonObject obj =  m_process_params.find("vrs")->toObject();
    m_process.visibleSensor.grade = obj.value("grade").toInt();
    m_process.visibleSensor.slit =  obj.value("slit").toInt();
    QJsonArray arr = obj.value("lambdas").toArray();
    for(const auto &it:qAsConst(arr))m_process.visibleSensor.lambdas.push_back(it.toInt());

    obj = m_process_params.find("irs")->toObject();
    m_process.infraRedSensor.grade = obj.value("grade").toInt();
    m_process.infraRedSensor.slit =  obj.value("slit").toInt();


    arr = obj.value("lambdas").toArray();
    for(const auto &it:qAsConst(arr))m_process.infraRedSensor.lambdas.push_back(it.toInt());


    obj = m_process_params.find("angles")->toObject();
    ui->doubleSpinBox_angleBigSphere->setValue(obj.value("bigSphere").toDouble());
    ui->doubleSpinBox_angleSmallSphere->setValue(obj.value("smallSphere").toDouble());
    ui->doubleSpinBox_angleSafetyLeft->setValue(obj.value("leftSafety").toDouble());
    ui->doubleSpinBox_angleLinearLights->setValue(obj.value("linearLight").toDouble());
    ui->doubleSpinBox_angleSafetyRight->setValue(obj.value("rightSafety").toDouble());

    m_anglesScene.bigSphere = ui->doubleSpinBox_angleBigSphere->value();
    m_anglesScene.smallSphere = ui->doubleSpinBox_angleSmallSphere->value();
    m_anglesScene.linearLight = ui->doubleSpinBox_angleLinearLights->value();
    m_anglesScene.leftSafety = ui->doubleSpinBox_angleSafetyLeft->value();
    m_anglesScene.rightSafety = ui->doubleSpinBox_angleSafetyRight->value();
    setUpScene();
    m_sceneCalibr->update();
    m_isResetZero = false;


    for(auto it:qAsConst(m_process.visibleSensor.lambdas)) qDebug()<<"Lambda vs:"<<it;
    for(auto it:qAsConst(m_process.infraRedSensor.lambdas))qDebug()<<"Lambda ir:"<<it;
    qDebug()<<"vs grade: "<<m_process.visibleSensor.grade;
    qDebug()<<"irs grade: "<<m_process.infraRedSensor.grade;

}

void MainWindow::getExpositions()
{
    QJsonObject expositions = readJsonDocumentFromFile(QDir::currentPath()+"/expositions.json").object();

    qDebug()<<"Read expositions JSON.....";
    expositions_for_grade_1200 =  expositions.find("1200")->toObject();
    expositions_for_grade_300 = expositions.find("300")->toObject();

    //Read expositions test
    QStringList lamps = expositions_for_grade_1200.keys();
    qDebug()<<"lamps size: "<<lamps.size();

    for(int i=0;i<lamps.size();++i){
        QJsonObject lamp = expositions_for_grade_1200.find(lamps[i])->toObject();
        QStringList expositionsKeys = lamp.keys();
        QVector<QMap<QString,double>> lampValues;
        qDebug()<<"************** Lamp name:"<<lamps.at(i)<<"\n";
        for(const auto &it:qAsConst(expositionsKeys)){
            qDebug()<<it<<lamp.value(it).toDouble();
            auto map = QMap<QString,double>();
            map.insert(it,lamp.value(it).toDouble());

        }
        m_expositions_for_grade1200.insert(lamps[i],lampValues);
    }
    qDebug()<<"grade1200: "<<m_expositions_for_grade1200.size();
    //obj.value("bigSphere").toDouble();
    //QJsonObject m_process_params = readJsonDocumentFromFile(QDir::currentPath()+"/calibr_process.json").object();
    //QJsonObject obj =  m_process_params.find("vrs")->toObject();
    //qDebug()<<"Get expositions from json: "<<getExpositionFromJson();
}

void MainWindow::showSpectralPlot(QCustomPlot *cp, SensorType st, QVector<double> *data, double max, double min)
{

    Q_UNUSED(st);
    QVector<double>spectr = *data;
    QVector <double> channels;
    for(int i=1;i<=spectr.size();++i){
        channels.push_back(i);
    }
    cp->clearGraphs();
    cp->addGraph();
    cp->graph(0)->setPen(QPen(QColor("green"),2));
    cp->graph(0)->setData(channels,spectr);
    cp->xAxis->setTickLabelRotation(30);
    cp->xAxis->ticker()->setTickCount(9);
    cp->yAxis->setRange(min, max);
    cp->xAxis->setRange(0, spectr.count());
    cp->replot();
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

    QVector<QPair<QLabel*,bool>> labels = {
        {ui->label_test_power_1,m_powerManager->results().power1},
        {ui->label_test_power_2,m_powerManager->results().power2},
        {ui->label_test_power_3,m_powerManager->results().power3}
    };
    bool testOk = true;
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
    }else{
        m_sounder.playSound("network_error.mp3");
        ui->pushButton_Forward->setEnabled(false);
    }

}


void MainWindow::initializeVariables()
{
    m_expositionsList<<"8.352"    <<"16.704"  <<"25.056" <<"41.760"
                    <<"58.464"   <<"75.168"  <<"83.520" <<"91.872"
                   <<"150.336"  <<"192.096" <<"208.800" <<"217.152"
                  <<"258.912"  <<"283.968" <<"292.320" <<"501.120"
                 <<"601.344"  <<"701.568" <<"902.016" <<"1002.240"
                <<"1102.464" <<"1302.912"<<"1403.136" <<"1503.360";
    ui->comboBox_exposition->addItems(m_expositionsList);

    grades = {{1200,0},{600,1},{300, 2}};



    solarResults = {false,false,false,false,false};

    QStringList lightSource = {"Малая сфера","Большая сфера","Лазер"};
    QStringList pages = {
        "Приветствие","Техника безопасности","Тестирование",
        "Подготовка к калибровке",
        "Калибровка","Настройка параметров","Расчёт","Результат"
    };
    for(int i=0;i<pages.size();++i) m_pages.insert(i,pages.at(i));

    m_testsResults = {false,false,false,false,false,false,false};
    m_isTestPassed = false;

    m_isGraphUpdate = false;
    m_isControlPressed = false;
    m_lampsCounter = 6;

}

void MainWindow::createObjects()
{


    m_powerManager = new PowerSupplyManager;
    m_ini = new QSettings(QDir::currentPath()+"/kameya.ini",QSettings::IniFormat);
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
    setUpGraph(ui->widget_spectrData_tuneUp);

    m_movie.setFileName(":/guiPictures/calculation_process.gif");
    ui->label_calculation->setMovie(&m_movie);
    m_movie.start();
    QStringList sensors = {"ПВД","ПИК"};
    ui->comboBox_sensor->addItems(sensors);
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::setUpGraph(QCustomPlot *widget)
{
    QColor fontColor(0,128,0);
    QColor backgroundColor(0,0,0);

    widget->xAxis->setBasePen(QPen(fontColor, 1));
    widget->yAxis->setBasePen(QPen(fontColor, 1));
    widget->xAxis->setTickPen(QPen(fontColor, 1));
    widget->yAxis->setTickPen(QPen(fontColor, 1));
    widget->xAxis->setSubTickPen(QPen(fontColor, 1));
    widget->yAxis->setSubTickPen(QPen(fontColor, 1));
    widget->xAxis->setTickLabelColor(fontColor);
    widget->yAxis->setTickLabelColor(fontColor);
    widget->xAxis->grid()->setPen(QPen(fontColor, 1, Qt::DotLine));
    widget->yAxis->grid()->setPen(QPen(fontColor, 1, Qt::DotLine));
    widget->xAxis->setLabelColor(fontColor);
    widget->yAxis->setLabelColor(fontColor);
    widget->setBackground(backgroundColor);
}

void MainWindow::setUpScene()
{
    ui->graphicsView->setScene(m_sceneCalibr);
    ot = new OpticTable;
    m_sceneCalibr->addItem(ot);
    ot->setAngles(m_anglesScene);
    ot->setAngle(m_anglesScene.bigSphere);
}

void MainWindow::makeConnects()
{
    qRegisterMetaType<QVector<double>>();
    qRegisterMetaType<PowerOuts>();
    qRegisterMetaType<Param>();
    qRegisterMetaType<PowerDevice>();
    qRegisterMetaType<DeviceParam>();

    connect(m_powerManager,SIGNAL(allLampsSwitchedOn()),&m_afterLampsOnHeatingTimer,SLOT(start()));
    connect(m_powerManager,SIGNAL(oneLampWasSwitchedOff()),this,SLOT(afterLampWasSwitchedOff()));
    connect(m_powerManager,&PowerSupplyManager::timeOutCaseWasHappened,this,&MainWindow::timeOutCaseHandler);
}

void MainWindow::showShutterState()
{

}

void MainWindow::showFilterState()
{

}

void MainWindow::showGratingState()
{
}

void MainWindow::showCurrentWavelength()
{

}

void MainWindow::getSingleSpectr4ActiveSensor()
{
    switch(m_activeSensor){
    case VISIBLE_RANGE_SENSOR:emit requestVisibleSpectr();
        break;
    case INFRA_RED_SENSOR: emit requestInfraredSpectr();
        break;
    case UKNOWN_SENSOR:
        break;}

}

void MainWindow::openFolderInExplorer()
{
    QString openExplorer = "c:/windows/explorer.exe";
    QStringList args;
    args.append(QDir::toNativeSeparators(QDir::currentPath()));
    QProcess::startDetached(openExplorer, args);
}

void MainWindow::afterDarkReady()
{

}

void MainWindow::checkShutterState()
{

}

void MainWindow::saveAnglesToJson()
{
    QFile file(QDir::currentPath()+"\\calibr_process.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonParseError JsonParseError;
    QJsonDocument JsonDocument = QJsonDocument::fromJson(file.readAll(), &JsonParseError);
    file.close();
    QJsonObject RootObject = JsonDocument.object();
    QJsonValueRef ref = RootObject.find("angles").value();
    QJsonObject m_addvalue = ref.toObject();

    m_addvalue.insert("leftSafety", ui->doubleSpinBox_angleSafetyLeft->value());//set the value you want to modify
    m_addvalue.insert("bigSphere",  ui->doubleSpinBox_angleBigSphere->value());
    m_addvalue.insert("linearLight",ui->doubleSpinBox_angleLinearLights->value());
    m_addvalue.insert("rightSafety",ui->doubleSpinBox_angleSafetyRight->value());
    m_addvalue.insert("smallSphere",ui->doubleSpinBox_angleSmallSphere->value());
    ref=m_addvalue; //assign the modified object to reference
    JsonDocument.setObject(RootObject); // set to json document

    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    file.write(JsonDocument.toJson());
    file.close();

}

void MainWindow::continueCalibrProcess()
{

    if(m_gettingSpecs == SpectralGetting::CapturingBlacks){
        isStop = false;
        m_gettingSpecs =   SpectralGetting::CapturingReal;
        setMetaData();
        getSingleSpectr4ActiveSensor();
        return;
    };
    if(m_gettingSpecs == SpectralGetting::CapturingReal){

        if(m_logic.lightSource == LightSource::BIG_SPHERE){
            changeWave();//Start getting data for new lambda
            return;
        }
        if(m_logic.lightSource == LightSource::SMALL_SPHERE){
            m_logic.lightSource = LightSource::BIG_SPHERE;
            ot->setAngle(Angles.key(srcNames.bigSphere));
            m_sceneCalibr->update();
            return;
        }
        if(m_logic.lightSource == LightSource::LINEAR_LIGHTS){
            m_logic.lightSource = LightSource::SMALL_SPHERE;
            ot->setAngle(Angles.key(srcNames.smallSphere));
            m_sceneCalibr->update();
            return;
        }

    }



}

void MainWindow::afterLampWasSwitchedOff()
{
    m_logic.calibrAction = CalibrSteps::AFTER_SWITCH_OFF_ONE_LAMP; 
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
    ui->label_afterHeatingTime->setText(QDateTime::fromSecsSinceEpoch(time).toUTC().toString("hh:mm:ss"));

}

void MainWindow::anglePositionIsReached()
{
    if(m_logic.calibrAction == CalibrSteps::GET_OPTIMAL_EXPO){

        m_isAngleReached = true;
        getSingleSpectr4ActiveSensor();

    };
    if(m_logic.calibrAction == CalibrSteps::GATHERING_DATA){

        startMeasuringAfterGatheringExpositions();

    }
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
            m_powerManager->connectToHost();
            QTimer::singleShot(5000,this,SLOT(testSlot()));
        }

        if(m_pages.value(ui->stackedWidget->currentIndex())=="Подготовка к калибровке"){


            m_sounder.playSound("preparations.mp3");
            QTimer::singleShot(5000,this,[=](){
                m_sounder.playSound("switchOnLamps.mp3");
                m_powerManager->startToReachTargetCurrent();
            });

        }

        if(m_pages.value(ui->stackedWidget->currentIndex())=="Калибровка"){

            m_sounder.playSound("startCalibration.mp3");
            isEnd = true;

        }

        if(m_pages.value(ui->stackedWidget->currentIndex())=="Расчёт")m_sounder.playSound("calculation.mp3");

        if(m_pages.value(ui->stackedWidget->currentIndex())=="Результат")m_sounder.playSound("finishCalibration.mp3");

    }
}

void MainWindow::on_pushButton_angleBigSphere_clicked()
{
    m_sounder.playSound("bigSphereAngle.mp3");
}

void MainWindow::on_pushButton_angleSmallSphere_clicked()
{
    m_sounder.playSound("smallSphereAngle.mp3");
}

void MainWindow::on_pushButton_angleLasers_clicked()
{
    m_sounder.playSound("linearLightsAngle.mp3");
}

void MainWindow::on_pushButton_angleRightSafety_clicked()
{
    double angle = ui->doubleSpinBox_angleSafetyRight->value();
    m_sounder.playSound("rightSafetyAngle.mp3");
}

void MainWindow::on_pushButton_zero_clicked()
{
}

void MainWindow::on_checkBox_5_cooling_for_big_sphere_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        m_sounder.playSound("dontForgetCooling.mp3");
    };
}

void MainWindow::on_pushButton_switchOffOneLamp_clicked()
{
    //ui->pushButton_switchOffOneLamp->setEnabled(false);
    m_sounder.playSound("switchOffLamp.mp3");
    m_powerManager->switchOffOneLamp();
}

void MainWindow::timeOutCaseHandler()
{
    ui->pushButton_switchOffOneLamp->setEnabled(true);
    qDebug()<<"TimeOut handler....";
}
