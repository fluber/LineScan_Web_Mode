#ifndef LISTENTHREAD_H
#define LISTENTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>

class ListenThread : public QThread
{
    Q_OBJECT
public:
    explicit ListenThread(QTcpSocket *socket,  QObject *parent = nullptr);
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void receive_message(const QString &message);

public slots:
    void readyRead();
    void disconnected();

private:
    QTcpSocket *m_socket;

};

#endif // LISTENTHREAD_H
