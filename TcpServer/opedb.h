#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QStringList>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getinstance();
    void init();
    ~OpeDB();

    bool handleRegist(const char* name, const char* pwd);
    bool handleLogin(const char* name, const char* pwd);
    void handleOffline(const char* name);
    QStringList handleAllOnline();
    int handleSearchUsr(const char* name);
    int handleAddFriend(const char* pername, const char* name);
    void handleAddFriendAgree(const char *addedName, const char *sourceName);
    int getIdByUserName(const char *name);

signals:

public slots:

private:
    QSqlDatabase m_db;  // 连接数据库
};

#endif // OPEDB_H
