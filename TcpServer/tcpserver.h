#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QHostAddress>
#include <QTcpServer>
#include <QHash>
#include "tcpsocket.h"

//继承QTCPSERVER以实现多线程TCPscoket的服务器。
//如果socket的信息处理直接处理的话，很多新建的信号和槽是用不到的
class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(int numConnections = 10000, QObject *parent = nullptr);
    ~TcpServer();

    void setMaxPendingConnections(int numConnections);//重写设置最大连接数函数
    void incomingConnection(qintptr socketDescriptor);//覆盖已获取多线程

signals:
    void connectClient(qint64, const QString &, quint16);//发送新用户连接信息
    void sockDisConnect(qint64, const QString &, quint16);//断开连接的用户信息
    void sentDisConnect(qint64); //断开特定连接，并释放资源，-1为断开所有。

public slots:
    void clear(); //断开所有连接，线程计数器请0
private slots:
    void sockDisConnectSlot(qint64 socketDescriptor,const QString &ip, quint16 port, QThread *th);//断开连接的用户信息

private:
    QHash<qint64, TcpSocket *> tcpClient;//管理连接的map
    int maxConnections;
    int nConnectCount;
};
#endif // TCPSERVER_H
