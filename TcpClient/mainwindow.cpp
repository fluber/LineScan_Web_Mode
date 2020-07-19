#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_socket(new QTcpSocket(this))
    , m_settings(new QSettings(qApp->applicationDirPath() + "/settings.txt", QSettings::IniFormat))
{
    ui->setupUi(this);
    ui->edtPort->setText(m_settings->value("port", 23888).toString());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if (ui->edtPort->text().length() > 0)
    {
        m_socket->connectToHost(QHostAddress::LocalHost, ui->edtPort->text().toInt());

        if(!m_socket->waitForConnected(5000))
        {
            qDebug() << "Error: " << m_socket->errorString();
        }
        m_settings->setValue("port", ui->edtPort->text());
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->pushButton_4->setEnabled(true);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    m_socket->disconnectFromHost();
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
}

void MainWindow::on_pushButton_3_clicked()
{
     m_socket->write("GO");
}

void MainWindow::on_pushButton_4_clicked()
{
     m_socket->write("Stop");

}
