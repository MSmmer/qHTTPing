#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    MainWindow w;
    w.show();

    int rc = a.exec();

    curl_global_cleanup();

    return rc;
}
