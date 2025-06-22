#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);

    loadConfig();

    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig(){
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray data = file.readAll();
        QString strData(data);
        // qDebug() << strData;
        file.close();

        strData.replace("\r\n", " ");  // 替换掉\r\n
        // qDebug() << strData;

        QStringList strList = strData.split(" "); // 按空格切分
        // for(int i = 0; i < strList.size(); i++){
        //     qDebug() << "--->" << strList[i];
        // }
        m_strIP = strList[0];
        m_usPort = strList.at(1).toUShort(); //读取的类型是QString，要转换成quint16
        // qDebug() << "ip:" << m_strIP << " port:" << m_usPort;
    }
    else{
        QMessageBox::critical(this, "open config", "open config failed");
    }
}

