#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Доступ к лог-окну
    QPlainTextEdit* logWindow;

    // Статический указатель для доступа из messageHandler
    static MainWindow* instance;
};

#endif // MAINWINDOW_H
