#include "listenthread.h"

ListenThread::ListenThread(QTcpSocket *socket, QObject *parent) :
    QThread(parent),
    m_socket(socket)

{
}

void ListenThread::run()
{
    // thread starts here
    qDebug() << " Thread started";



    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    exec();
}

void ListenThread::readyRead()
{
    // get the information
    QByteArray Data = m_socket->readAll();
    qDebug() << Data;
    Q_EMIT receive_message(QString(Data));
}

void ListenThread::disconnected()
{
    qDebug() << "client disconnect";
    m_socket->deleteLater();
    exit(0);
}
