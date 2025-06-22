#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

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

signals:

public slots:

private:
    QSqlDatabase m_db;  // 连接数据库
};

#endif // OPEDB_H
