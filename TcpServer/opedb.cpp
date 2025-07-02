#include "opedb.h"
#include <QMessageBox>
#include <QDebug>

OpeDB::OpeDB(QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

OpeDB &OpeDB::getinstance()
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("C:\\Users\\14376\\Documents\\QT\\TcpServer\\cloud.db");
    if(m_db.open()){
        QSqlQuery query;
        query.exec("select * from usrInfo");
        while(query.next()){
            QString data = QString("%1,%2,%3").arg(query.value(1).toString(), query.value(2).toString(), query.value(3).toString());
            qDebug() << data;
        }
    }else{
        QMessageBox::critical(NULL, "打开数据库", "打开数据库失败");
    }

}

OpeDB::~OpeDB()
{
    m_db.close();
}

bool OpeDB::handleRegist(const char *name, const char *pwd)
{
    if(name == nullptr || pwd == nullptr){
        qDebug() << "name | pwd is NULL";
        return false;
    }
    QString data = QString("insert into usrInfo(name, pwd) values(\'%1\', \'%2\')").arg(name, pwd);
    QSqlQuery query;
    return query.exec(data);
}

bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    if(name == nullptr || pwd == nullptr){
        qDebug() << "name | pwd is NULL";
        return false;
    }
    QString data = QString("select * from usrInfo where name = \'%1\' and pwd = \'%2\' and online = 0").arg(name, pwd);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    if(query.next()){ // 只查询到一条数据则为真
        QString data = QString("update usrInfo set online = 1 where name = \'%1\' and pwd = \'%2\'").arg(name, pwd);
        qDebug() << data;
        QSqlQuery query;
        query.exec(data);
        return true;
    }else{
        return false;
    }
}

void OpeDB::handleOffline(const char *name)
{
    if(name == nullptr){
        qDebug() << "name is NULL";
        return;
    }
    QString data = QString("update usrInfo set online = 0 where name = \'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
}

QStringList OpeDB::handleAllOnline()
{
    QString data = QString("select name from usrInfo where online = 1");
    QSqlQuery query;
    query.exec(data);

    QStringList result;
    result.clear();

    while(query.next()){
        result.append(query.value(0).toString());
    }

    return result;
}

int OpeDB::handleSearchUsr(const char *name)
{
    if(name == nullptr){
        return -1;
    }
    QString data = QString("select online from usrInfo where name = \'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        int ret = query.value(0).toInt();
        return ret;
    }else{
        return -1;
    }
}

int OpeDB::handleAddFriend(const char *pername, const char *name)
{
    if(pername == nullptr || name == nullptr){
        return -1;
    }
    QString data = QString(
        "SELECT * FROM friend WHERE "
        "(id = (SELECT id FROM usrInfo WHERE name = '%1') AND friendId = (SELECT id FROM usrInfo WHERE name = '%2')) "
        "OR "
        "(id = (SELECT id FROM usrInfo WHERE name = '%3') AND friendId = (SELECT id FROM usrInfo WHERE name = '%4'))"
        ).arg(pername, name, name, pername);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        return 0; // 双方已经是好友
    }
    data = QString("select online from usrInfo where name = \'%1\'").arg(pername);
    query.exec(data);
    if(query.next()){
        int ret = query.value(0).toInt();
        if(ret == 1){
            return 1; // 在线
        }
        else if(ret == 0){
            return 2; // 不在线
        }
    }
    return 3; // 用户不存在
}

void OpeDB::handleAddFriendAgree(const char *addedName, const char *sourceName)
{
    if(addedName == nullptr || sourceName == nullptr){
        return;
    }
    int sourceUserId = getIdByUserName(sourceName);
    int addedUserId = getIdByUserName(addedName);
    qDebug() << sourceUserId << " " << addedUserId;
    QString strQuery = QString("insert into friend values(%1, %2) ").arg(sourceUserId).arg(addedUserId);
    QSqlQuery query;
    query.exec(strQuery);
}

int OpeDB::getIdByUserName(const char *name)
{
    if(name == nullptr){
        return -1; // 不存在
    }
    QString data = QString("select id from usrInfo where name=\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    qDebug() << data;
    if(query.next()){
        return query.value(0).toInt();
    }
    else{
        qDebug() << "error";
        return -1;
    }
}
