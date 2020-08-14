#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>
#include "constant.h"

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpSocket(QObject *parent = nullptr);
    ~TcpSocket();

    void connectTcpServer(const QString& ip, quint16 port);
signals:
    void dataPacketReady(SocketPacket socketData);

public slots:
    void send(QByteArray& data, const QByteArray& dataType);
private slots:
    void OnReadyRead();

private:
    bool parsePacket(SocketPacket *packet);

private:
    SocketPacket m_dataPacket;
};

#endif // TCPSOCKET_H
