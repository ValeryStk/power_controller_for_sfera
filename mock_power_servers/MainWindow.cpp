#include "MainWindow.h"

MainWindow* MainWindow::instance = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    instance = this;

    logWindow = new QPlainTextEdit(this);
    logWindow->setReadOnly(true);
    setCentralWidget(logWindow);
}

MainWindow::~MainWindow()
{
    instance = nullptr;
}
