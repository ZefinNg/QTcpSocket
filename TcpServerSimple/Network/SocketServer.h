/***********************************************************

  class:SocketServer

  作者：lth
  日期：2016-03-15
  描述：收发Socket数据包。

************************************************************/

#ifndef SOCKETCOMMUNICATION_H
#define SOCKETCOMMUNICATION_H
#include <QObject>
#include <QTcpServer>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QAbstractSocket>
#include "SocketConstant.h"
#include <QTimer>
#include <QFile>

class SocketServer: public QObject
{
    Q_OBJECT

private:
    QTcpServer  *m_tcpServer;
    QTcpSocket  *m_dataSocket; //收发数据
    SocketPacket m_dataPacket;
    QTimer       m_diagnoseTimer; //接收到握手信号在一定间隔时间再判断是否还能接收到

public:
    static SocketServer *getInstance();
    ~SocketServer();

protected:
    SocketServer();

private:
    bool parsePacket(QTcpSocket *socket, SocketPacket *packet);

signals:
    void socketConnected(bool); //客户端断开/连接时发送
    void dataPacketReady(SocketPacket socketData);

public slots:
    void send(QByteArray data,QByteArray dataType);

private slots:
    void OnNewConnection();
    void OnReadyRead();
    void OnDisconnected();
    void socketTimeOut();
};

#endif // SOCKETCOMMUNICATION_H
