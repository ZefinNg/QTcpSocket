#include "tcpsocket.h"

TcpSocket::TcpSocket(QObject *parent)
    : QTcpSocket (parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));
}

TcpSocket::~TcpSocket()
{
    this->disconnectFromHost();
    if (this->state() != QAbstractSocket::UnconnectedState) {
        this->waitForDisconnected();
    }
}

void TcpSocket::connectTcpServer(const QString& ip, quint16 port)
{
    this->connectToHost(ip, port);
    this->waitForConnected();
}

void TcpSocket::OnReadyRead()
{
    m_dataPacket.data += this->readAll();

    // 循环解析包数据，m_dataPacket.data 中可能不只一包数据
    bool isOk = false;
    do{
        isOk = parsePacket(&m_dataPacket);
    }while(isOk);
}

// 解包
bool TcpSocket::parsePacket(SocketPacket *packet)
{
    int pIndexStart = packet->data.indexOf(NET_PACKET_START);
    if(pIndexStart < 0)
    {
        return false;
    }

    packet->data = packet->data.mid(pIndexStart); //截取从包头index_start到末尾的数据
    SocketPacket tmpPacket;
    tmpPacket.data = packet->data;

    tmpPacket.data.remove(0, QByteArray(NET_PACKET_START).size());//删除包头

    //解析包长度
    if(tmpPacket.data.count() < NET_PACKET_LENGTH_BYTES)
    {
        return false;
    }
    bool isOk;
    tmpPacket.length = tmpPacket.data.mid(0, NET_PACKET_LENGTH_BYTES).toLong(&isOk);
    if(isOk == false)
    {
        packet->data.remove(0, QByteArray(NET_PACKET_START).size());//删除包头
        if(packet->data.indexOf(NET_PACKET_START) >= 0)
        {
            return true;//有可能出现粘包的情况，继续解析后面数据
        }
        else
        {
            return false;
        }
    }

    //数据到达包长度
    tmpPacket.data.remove(0, NET_PACKET_LENGTH_BYTES);//删除数据长度
    if(tmpPacket.length > tmpPacket.data.count())
    {
        return false;
    }

    //包尾是否匹配
    tmpPacket.data.resize(tmpPacket.length);//删除多余数据
    if(tmpPacket.data.endsWith(NET_PACKET_END) == false)
    {
        packet->data.remove(0, QByteArray(NET_PACKET_START).size());//删除包头
        if(packet->data.indexOf(NET_PACKET_START) >= 0)
        {
            return true;//有可能出现粘包的情况，继续解析后面数据
        }
        else
        {
            return false;
        }
    }

    tmpPacket.data.resize(tmpPacket.length -
                          QByteArray(NET_PACKET_END).count()); //删除包尾

    //解析出数据类型
    if(tmpPacket.data.count() < NET_PACKET_TYPE_BYTES)
    {
        return false;
    }
    QByteArray dataType = tmpPacket.data.left(NET_PACKET_TYPE_BYTES);
    tmpPacket.dataType = dataType;

    tmpPacket.data.remove(0, NET_PACKET_TYPE_BYTES);//删除数据类型

    //删除当前包数据
    packet->data.remove(0,
                        QByteArray(NET_PACKET_START).size()
                        + NET_PACKET_LENGTH_BYTES
                        + tmpPacket.length);

    //发送数据包消息
    emit dataPacketReady(tmpPacket);

    return true;
}

// 封包并发送
void TcpSocket::send(QByteArray &data, const QByteArray &dataType)
{
    if(this->state() !=  QTcpSocket::ConnectedState)
    {
        return;
    }

    data.insert(0, dataType); //[包类型 + 数据 + 包尾]
    data.append(NET_PACKET_END);

    //长度占8字节，前面补零，如"00065536"
    int size = data.size();
    QByteArray length = QByteArray().setNum(size);
    length = QByteArray(NET_PACKET_LENGTH_BYTES, '0') + length;
    length = length.right(NET_PACKET_LENGTH_BYTES);

    data.insert(0, NET_PACKET_START + length);//插入 [包头 + 数据长度]

    this->write(data);
}
