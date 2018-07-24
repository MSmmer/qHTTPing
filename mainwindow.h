#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

extern "C" {
#include <curl/curl.h>
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void timerHandler();

private slots:
    void on_GO_clicked();

    void on_EXIT_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    CURL *curl;

    int pingNr, ok, fail;
    long unsigned int totalMs;
};

#endif // MAINWINDOW_H
