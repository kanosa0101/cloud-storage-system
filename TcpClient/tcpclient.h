#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include <protocol.h>
#include <opewidget.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpClient;
}
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    void loadConfig();

    static TcpClient& getinstance(); // 使用单例模式方便不同模块访问同一个TCP连接
    QTcpSocket& getTcpSocket();
    QString loginName();

public slots:
    void showConnect();
    void recvMsg();

private slots:
    // void on_pushButton_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;

    //连接服务器，和服务器数据交互
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;
};
#endif // TCPCLIENT_H
