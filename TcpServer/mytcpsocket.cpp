#include "mytcpsocket.h"
#include <QDebug>
#include "protocol.h"
#include "opedb.h"
#include "mytcpserver.h"

MyTcpSocket::MyTcpSocket() {
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg()));
    connect(this, SIGNAL(disconnected()), this, SLOT(clientOffline()));
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::recvMsg()
{
    qDebug() << this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU* pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint)); // 从PDU后一个uint处开始读取数据
    switch (pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_REQUEST:{
        char caName[32] =  {'\0'};
        char caPwd[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);
        strncpy(caPwd, pdu->caData + 32, 32);
        bool ret = OpeDB::getinstance().handleRegist(caName, caPwd);
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
        if(ret){
            strcpy(respdu->caData, REGIST_OK);
        }else{
            strcpy(respdu->caData, REGIST_FAILED);
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = nullptr;
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:{
        char caName[32] =  {'\0'};
        char caPwd[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);
        strncpy(caPwd, pdu->caData + 32, 32);
        bool ret = OpeDB::getinstance().handleLogin(caName, caPwd);
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
        if(ret){
            strcpy(respdu->caData, LOGIN_OK);
            m_strName = caName;
        }else{
            strcpy(respdu->caData, LOGIN_FAILED);
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = nullptr;
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLLINE_REQUEST:{
        QStringList ret = OpeDB::getinstance().handleAllOnline(); // 不直接发送 ret 是因为 ret 不连续
        uint uiMsgLen = ret.size() * 32;
        PDU* respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLLINE_RESPOND;
        for(int i = 0; i < ret.size(); i++){
            memcpy((char*)(respdu->caMsg) + i * 32, ret.at(i).toStdString().c_str(), ret.at(i).size());
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = nullptr;
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:{
        int ret = OpeDB::getinstance().handleSearchUsr(pdu->caData);
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        if(ret == -1){
            strcpy(respdu->caData, SEARCH_USR_NO);
        }else if(ret == 1){
            strcpy(respdu->caData, SEARCH_USR_ONLINE);
        }else if (ret == 0){
            strcpy(respdu->caData, SEARCH_USR_OFFLINE);
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = nullptr;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        char caPerName[32] =  {'\0'};
        char caName[32] = {'\0'};
        strncpy(caPerName, pdu->caData, 32);
        strncpy(caName, pdu->caData + 32, 32);
        int ret = OpeDB::getinstance().handleAddFriend(caPerName, caName);
        PDU* respdu = nullptr;
        switch (ret) {
        case -1:{
            respdu = mkPDU(0);
            respdu ->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, UNKNOWN_ERROR);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case 0:{
            respdu = mkPDU(0);
            respdu ->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, EXISTED_FRIEND);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case 1:{
            MyTcpServer::getInstance().resend(caPerName, pdu);
            break;
        }
        case 2:{
            respdu = mkPDU(0);
            respdu ->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case 3:{
            respdu = mkPDU(0);
            respdu ->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, ADD_FRIEND_NOEXIST);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        default:
            break;
        }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:{
        char addedName[32] = {'\0'};
        char sourceName[32] = {'\0'};
        strncpy(addedName, pdu->caData, 32);
        strncpy(sourceName, pdu->caData + 32, 32);
        PDU *respdu = mkPDU(0);
        OpeDB::getinstance().handleAddFriendAgree(addedName, sourceName);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;

        MyTcpServer::getInstance().resend(sourceName, pdu);
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = nullptr;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:{
        char sourceName[32] = {'\0'};
        strncpy(sourceName, pdu -> caData + 32, 32);
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;

        MyTcpServer::getInstance().resend(sourceName, pdu);
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = nullptr;
        break;
    }
    default:
        break;
    }

    free(pdu);
    pdu = nullptr;
}

void MyTcpSocket::clientOffline()
{
    OpeDB::getinstance().handleOffline(m_strName.toStdString().c_str()); // 从 QString 转换为 const char*
    emit offline(this);
}
