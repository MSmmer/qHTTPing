#include <QTime>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#if 0
    try {
        QIcon logo("qHTTPing.png");
        QWidget::setWindowIcon(logo);
    }
    catch(...) {
        qDebug("Could not set logo icon");
    }
#endif

    ui->interval->setMinimum(0.01);
    ui->interval->setValue(0.5);
    ui->interval->setMaximum(99999);
    ui->interval->setSuffix("s");

    ui->timeout->setMinimum(0.01);
    ui->timeout->setValue(5);
    ui->timeout->setMaximum(99999);
    ui->timeout->setSuffix("s");

    ui->doHeadRequest->setChecked(true);

    ui->counts->setText("");

    curl = NULL;

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(timerHandler()));
}

MainWindow::~MainWindow()
{
    timer -> stop();
    delete timer;

    delete ui;

    if (curl)
    curl_easy_cleanup(curl);
}

void MainWindow::on_GO_clicked()
{
    if (curl) {
        timer -> stop();

        ui->GO->setText("Go");
        ui->options->setEnabled(true);

        curl_easy_cleanup(curl);
        curl = NULL;
    }
    else {
        ui->GO->setText("Stop");
        ui->options->setEnabled(false);

        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, !!(ui->skipPeerVerify->checkState() == Qt::Checked));
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, !!(ui->skipHostNameVerify->checkState() == Qt::Checked));
        curl_easy_setopt(curl, CURLOPT_URL, ui->URL->text().toLatin1().data());
        curl_easy_setopt(curl, CURLOPT_NOBODY, !!(ui->doHeadRequest->checkState() == Qt::Checked));

        curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, !!(ui->pipelining->checkState() != Qt::Checked));
        curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, !!(ui->pipelining->checkState() != Qt::Checked));

        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, ui->httpVersion->checkState() == Qt::Checked ? CURL_HTTP_VERSION_2_0 : (ui->pipelining->checkState() == Qt::Checked ? CURL_HTTP_VERSION_1_1 : CURL_HTTP_VERSION_1_0));

        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, long(ui->timeout->text().toDouble() * 1000.0));

        curl_easy_setopt(curl, CURLOPT_USERAGENT, "qHTTPing");

        ok = fail = pingNr = 0;
        totalMs = 0;

        ui->results->clear();

        QString header = "Pinging: ";
        header += ui->URL->text();
        ui->results->append(header);

        timer -> setSingleShot(false);
        timer -> start(ui->interval->value() * 1000);
    }
}

void MainWindow::on_EXIT_clicked()
{
    timer -> stop();

    this -> close();
}

void MainWindow::timerHandler()
{
    QTime ts;
    ts.start();

    CURLcode rc = curl_easy_perform(curl);

    int ms = ts.elapsed();

    pingNr++;

    QString result;
    if (rc == CURLE_OK) {
        result = QString::asprintf("%d] elapsed: %dms", pingNr, ms);
        ok++;
        totalMs += ms;
    }
    else {
        result = QString::asprintf("failed: %s", curl_easy_strerror(rc));
        fail++;
    }

    ui->results->append(result);

    ui->counts->setText(QString::asprintf("ok: %d, failed: %d, avg: %.2fms", ok, fail, totalMs / double(ok)));

    qDebug("%s", result.toLatin1().data());
}
