#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    virtual ~MyTcpServer() = default;

    static MyTcpServer& getInstance();

    virtual void incomingConnection(qintptr socketDescriptor) override;

private:
    QList<MyTcpSocket*> m_tcpSocketList;


};

#endif // MYTCPSERVER_H
