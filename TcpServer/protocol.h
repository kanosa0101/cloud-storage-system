#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <qstring>
#include <unistd.h>

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed : name existed"
#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed : name or pwd error or relogin"

#define SEARCH_USR_NO "no such people"
#define SEARCH_USR_ONLINE "online"
#define SEARCH_USR_OFFLINE "offline"

#define UNKNOWN_ERROR "unknown error"

#define EXISTED_FRIEND "existed friend"
#define ADD_FRIEND_OFFLINE "usr offline"
#define ADD_FRIEND_NOEXIST "usr not exist"

typedef unsigned int uint;

enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGIST_REQUEST, // 注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND, // 注册回复
    ENUM_MSG_TYPE_LOGIN_REQUEST, // 登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND, // 登录回复
    ENUM_MSG_TYPE_ALL_ONLLINE_REQUEST, // 在线用户请求
    ENUM_MSG_TYPE_ALL_ONLLINE_RESPOND, // 在线用户回复
    ENUM_MSG_TYPE_SEARCH_USR_REQUEST, // 搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND, // 搜索用户回复
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST, // 添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND, // 添加好友回复
    ENUM_MSG_TYPE_ADD_FRIEND_AGREE, // 同意添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE, // 拒绝添加好友
    ENUM_MSG_TYPE_MAX = 0x00ffffff
};

struct PDU {
    uint uiPDULen; // 总的协议数据单元大小
    uint uiMsgType; // 消息类型
    char caData[64];
    uint uiMsgLen; // 实际消息长度
    int caMsg[]; // 实际消息
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
