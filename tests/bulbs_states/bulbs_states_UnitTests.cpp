#include "bulbs_states_UnitTests.h"

#include "MainWindow.h"
#include "logger.h"
#include "ui_MainWindow.h"
#include <QDebug>


namespace{


} // end namespace


bulbs_states_UnitTests::bulbs_states_UnitTests()
{
}

void bulbs_states_UnitTests::initTestCase()
{
    // Инициализация перед запуском всех тестов
}

void bulbs_states_UnitTests::cleanupTestCase()
{
    // Очистка после выполнения всех тестов
}

void bulbs_states_UnitTests::init()
{
    // Инициализация перед каждым тестом

}

void bulbs_states_UnitTests::cleanup()
{
    // Очистка после каждого теста
}

void bulbs_states_UnitTests::bulbs_items_test()
{
    int argc = 0;
    QApplication app(argc, nullptr);
    QPixmap pixmap = QPixmap::fromImage(QImage(":/guiPictures/bug.svg"));
    QCursor customCursor(pixmap);
    QApplication::setOverrideCursor(customCursor);
    qInstallMessageHandler(myMessageOutput);
    MainWindow *main_window = new MainWindow;
    main_window->ui->stackedWidget->setCurrentIndex(2);
    bulb_state states[NUMBER_OF_LAMPS] = {bulb_state::ON,
                                         bulb_state::OFF,
                                         bulb_state::UNDEFINED,
                                         bulb_state::ON,
                                         bulb_state::OFF,
                                         bulb_state::UNDEFINED};
    main_window->m_bulbs_graphics_item->set_bulb_states(states);
    main_window->m_sceneCalibr->update();
    main_window->m_timer_to_update_power_states->start();
    QPushButton update_scene_button(main_window);
    update_scene_button.setText("Update");
    QObject::connect(&update_scene_button,
                     &QPushButton::clicked,
                     [main_window](){
      bulb_state  states[NUMBER_OF_LAMPS] = {    bulb_state::ON,
                                                 bulb_state::ON,
                                                 bulb_state::ON,
                                                 bulb_state::ON,
                                                 bulb_state::ON,
                                                 bulb_state::ON};
        main_window->m_bulbs_graphics_item->set_bulb_states(states);
        main_window->m_sceneCalibr->update();
    });
    main_window->show();
    app.exec();
}



QTEST_MAIN(bulbs_states_UnitTests)
