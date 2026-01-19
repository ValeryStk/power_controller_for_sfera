#ifndef SINGLE_APPLICATION_H
#define SINGLE_APPLICATION_H

#include "qsharedmemory.h"
#include "qsystemsemaphore.h"
#include "QMessageBox"

void check_single_application_is_running(){
QSystemSemaphore semaphore("<POWER_CONTROLLER>", 1);
    semaphore.acquire();
    QSharedMemory sharedMemory("<POWER_CONTROLLER 2>");
    bool is_running;
    if (sharedMemory.attach()) {
      is_running = true;
    } else {
      sharedMemory.create(1);
      is_running = false;
    }
    semaphore.release();
    if (is_running) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.setText("Приложение уже запущено.\nВы можете запустить только один экземпляр приложения.");
      msgBox.exec();
      return;
}
}

#endif // SINGLE_APPLICATION_H
