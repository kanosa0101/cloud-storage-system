#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer(QObject *parent) : QTcpServer(parent) {}

MyTcpServer& MyTcpServer::getInstance(){
    static  MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "new client connected";
    MyTcpSocket* pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);

    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket*)), this, SLOT(deleteSocket(MyTcpSocket*)));
}

void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    QList<MyTcpSocket*>::Iterator iter = m_tcpSocketList.begin();
    for(; iter != m_tcpSocketList.end(); iter++){
        if(mysocket == *iter){
            (*iter)->deleteLater();  // 先删除对象, 延迟删除，可能使用互斥锁or智能指针也可以？
            *iter = NULL;
            m_tcpSocketList.erase(iter); // erase 后迭代器会失效，所以后面必须break
            break;
        }
    }
    // C++ 风格的写法， 采用智能指针
    // auto iter = std::find(m_tcpSocketList.begin(), m_tcpSocketList.end(), mysocket);
    // if (iter != m_tcpSocketList.end()) {
    //     delete *iter;       // 先释放内存
    //     m_tcpSocketList.erase(iter);  // 再移除指针
    // }

    for(int i = 0; i < m_tcpSocketList.size(); i++){
        qDebug() << m_tcpSocketList.at(i)->getName();
    }
}

