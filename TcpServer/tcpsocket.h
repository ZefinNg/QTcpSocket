#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "constant.h"
#include <QTcpSocket>
#include <QByteArray>
#include <QTime>
#include <QHostAddress>
#include <QDebug>
#include <QThread>
#include <QFile>

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpSocket(qintptr socketDescriptor, QObject *parent = nullptr);
    ~TcpSocket();

signals:
    void sockDisConnect(qint64, const QString &, quint16, QThread *);//NOTE:断开连接的用户信息，此信号必须发出！线程管理类根据信号计数的
    void dataPacketReady(SocketPacket socketData);

public slots:
    void send(QByteArray& data, const QByteArray& dataType);
    void disConTcp(qint64 socketDescriptor);
private slots:
    void OnReadyRead();

private:
    bool parsePacket(SocketPacket *packet);
    void dealDataPacket(const SocketPacket &tmpPacket);

private:
    qintptr socketID;
    SocketPacket m_dataPacket;
    QMetaObject::Connection dis;
};

#endif // TCPSOCKET_H
