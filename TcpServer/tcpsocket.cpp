#include "tcpsocket.h"

TcpSocket::TcpSocket(qintptr socketDescriptor, QObject *parent)
    : QTcpSocket(parent), socketID(socketDescriptor)
{
    this->setSocketDescriptor(socketDescriptor);
    connect(this, &QTcpSocket::readyRead, this, &TcpSocket::OnReadyRead);
    dis = connect(this,&QTcpSocket::disconnected,
        [&](){
            emit sockDisConnect(socketID,this->peerAddress().toString(),this->peerPort(),QThread::currentThread());//发送断开连接的用户信息
            this->deleteLater();
        });
}

TcpSocket::~TcpSocket()
{

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
    dealDataPacket(tmpPacket);

    return true;
}

void TcpSocket::dealDataPacket(const SocketPacket &tmpPacket)
{
    if (tmpPacket.dataType == NET_PACKET_TYPE_SEND_FILE) {
        QByteArray block = tmpPacket.data;
        QDataStream in(&block, QIODevice::ReadOnly);
        NetSendFile stru;
        in >> stru;

        QFile file(dirPath+'/'+stru.m_filename);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(stru.m_bytearray);
            qDebug() << "socketDescriptor=" << socketID <<"receive file" << stru.m_filename;
        }
    }
    else if (tmpPacket.dataType == NET_PACKET_TYPE_SEND_TEXT) {
        QByteArray block = tmpPacket.data;
        QDataStream in(&block, QIODevice::ReadOnly);
        NetSendText stru;
        in >> stru;
        qDebug() << "socketDescriptor=" << socketID <<"receive text" << stru.m_text;
    }
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

void TcpSocket::disConTcp(qint64 socketDescriptor)
{
    if (socketDescriptor == socketID)
    {
        this->disconnectFromHost();
    }
    else if (socketDescriptor == -1) //-1为全部断开
    {
        disconnect(dis); //先断开连接的信号槽，防止二次析构
        this->disconnectFromHost();
        this->deleteLater();
    }
}
