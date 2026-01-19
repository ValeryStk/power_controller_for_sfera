#include "bulbs_states_UnitTests.h"
#include "MainWindow.h"
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
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::red);
    QCursor customCursor(pixmap);
    QApplication::setOverrideCursor(customCursor);
    MainWindow *main_window = new MainWindow;
    main_window->ui->stackedWidget->setCurrentIndex(2);
    main_window->m_bulbs_graphics_item->set_current_lamp_index(0);
    main_window->m_bulbs_graphics_item->setBulbOn(0);
    main_window->m_timer_to_update_power_states->start();
    main_window->show();
    app.exec();
}



QTEST_MAIN(bulbs_states_UnitTests)
