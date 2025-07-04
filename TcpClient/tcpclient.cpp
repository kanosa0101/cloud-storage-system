#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>



TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);

    resize(500,250);

    loadConfig();

    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(showConnect()));
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(recvMsg()));

    //连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig(){
    QFile file(":/client.config");
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

TcpClient &TcpClient::getinstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::loginName()
{
    return m_strLoginName;
}

void TcpClient::showConnect(){
    QMessageBox::information(this, "连接服务器", "连接服务器成功");
}

void TcpClient::recvMsg()
{
    qDebug() << m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU* pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint)); // 从PDU后一个uint处开始读取数据
    switch (pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_RESPOND:{
        if(strcmp(pdu->caData, REGIST_OK) == 0){
            QMessageBox::information(this, "注册", REGIST_OK);
        }
        else if(strcmp(pdu->caData, REGIST_FAILED) == 0){
            QMessageBox::warning(this, "注册", REGIST_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:{
        if(strcmp(pdu->caData, LOGIN_OK) == 0){
            QMessageBox::information(this, "登录", LOGIN_OK);
            OpeWidget::getInstance().show();
            this->hide();
        }
        else if(strcmp(pdu->caData, LOGIN_FAILED) == 0){
            QMessageBox::warning(this, "登录", LOGIN_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLLINE_RESPOND:{
        OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:{
        if(strcmp(SEARCH_USR_NO, pdu->caData) == 0){
            QMessageBox::information(this, "搜索", QString("%1: not exist:").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }else if (strcmp(SEARCH_USR_ONLINE, pdu->caData) == 0){
            QMessageBox::information(this, "搜索", QString("%1: online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }else if(strcmp(SEARCH_USR_OFFLINE, pdu->caData) == 0){
            QMessageBox::information(this, "搜索", QString("%1: offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        char caName[32] = {'\0'};
        strncpy(caName, pdu->caData + 32, 32);
        int ret = QMessageBox::information(this, "添加好友", QString("%1 want to add you as friend ?").arg(caName), QMessageBox::Yes, QMessageBox::No);
        PDU* respdu = mkPDU(0);
        memcpy(respdu->caData, pdu->caData, 64); //strcpy复制到\0，memcpy负责指定长度内存块
        if(ret == QMessageBox::Yes){
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
        }else{
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
        }
        m_tcpSocket.write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = nullptr;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
        QMessageBox::information(this, "添加好友", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:{
        QMessageBox::information(this, "添加好友", QString("%1 已同意您的好友申请！").arg(pdu->caData));
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:{
        QMessageBox::information(this, "添加好友", QString("%1 已拒绝您的好友申请！").arg(pdu->caData));
        break;
    }
    default:
        break;
    }

    free(pdu);
    pdu = nullptr;
}


void TcpClient::on_login_pb_clicked()
{
    m_strLoginName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!m_strLoginName.isEmpty() && !strPwd.isEmpty()){
        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData, m_strLoginName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = nullptr;
    }else{
        QMessageBox::critical(this, "登录", "登录失败：用户名或密码不能为空");
    }
}


void TcpClient::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty() && !strPwd.isEmpty()){
        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = nullptr;
    }else{
        QMessageBox::critical(this, "注册", "注册失败：用户名或密码不能为空");
    }
}


void TcpClient::on_cancel_pb_clicked()
{

}

