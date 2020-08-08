/***********************************************************
**
** 作者：lth
** 日期：2016-03-09
** 描述：收发Socket数据包。
**
************************************************************/

#ifndef SOCKETCOMMUNICATION_H
#define SOCKETCOMMUNICATION_H
#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include "SocketConstant.h"
#include <QTimer>
#include <QMap>

class SocketClient: public QObject
{
    Q_OBJECT

protected://构造函数是保护型的，此类使用单例模式
    SocketClient();

private:
    QTcpSocket  *m_dataSocket;  //收发数据

    QTimer       m_diagnoseTimer; //接收到握手信号，过一段时间再判断是否还接收到

    SocketPacket m_dataPacket;
    QHostAddress m_tcpServerIP;
    int          m_tcpServerPort;
    volatile bool m_isConnecting;

public:
    static SocketClient *getInstance();//使用单例模式
    bool isConnected();
    void connectTcpServer(QHostAddress ip, int port);

private:
    bool parsePacket(QTcpSocket *socket, SocketPacket *packet);


signals:
    void dataPacketReady(SocketPacket socketData);
    void socketConnected(bool);

public slots:
    void send(QByteArray data, QByteArray dataType);
    void OnReadyRead();
    void OnDisconnected();
    void socketTimeOut();
};

#endif // SOCKETCOMMUNICATION_H
