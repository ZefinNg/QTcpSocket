#include "SocketServer.h"
#include <QStringList>
#include <QDebug>
//#include "MyDebug.h"
#include <QCoreApplication>
#include <QFile>
#include <QIODevice>
#include <QFlags>

SocketServer::SocketServer()
{
    qDebug()<<"SocketServer() start";
    m_dataSocket  = nullptr;

    m_tcpServer = new QTcpServer();
    for(int i = 0; i < 20; i++)//尝试不同的端口号
    {
        if (m_tcpServer->listen( QHostAddress::Any, NET_TCP_PORT + i*NET_PORT_SPACE))
        {
            connect(m_tcpServer, SIGNAL(newConnection()),
                    this, SLOT(OnNewConnection()));
            break;
        }
        else
        {
            qDebug()<<"mytcpserver listen failed!!";
        }
    }

    connect(&m_diagnoseTimer,SIGNAL(timeout()),this,SLOT(socketTimeOut()));

   qDebug()<<"SocketServer() finished";
}

SocketServer::~SocketServer()
{
    delete m_tcpServer;
    delete m_dataSocket;
}

SocketServer *SocketServer::getInstance()
{
    static SocketServer instance;
    return &instance;
}

void SocketServer::OnNewConnection()
{
    qDebug()<<"start";
    QTcpSocket *newSocket = m_tcpServer->nextPendingConnection();

    if(this->m_dataSocket == NULL)
    {
        qDebug()<<"sendTestInfoSocket OnNewConnection";
        qDebug()<<"socketDescriptor="<<newSocket->socketDescriptor();
        this->m_dataSocket = newSocket;
        emit socketConnected(true);
    }
    else
    {
        if(newSocket != 0)
        {
            newSocket->abort();
        }
        qDebug()<<"OnNewConnection3";
        return;
    }
    connect(newSocket, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));
    connect(newSocket, SIGNAL(disconnected()), this, SLOT(OnDisconnected()));

   qDebug()<<"end";
}

//握手信号超时
void SocketServer::socketTimeOut()
{
//    LOG_HERE("");
//    MY_DEBUG(g_getNowTime("hh:mm:ss.zzz")<<"socketTimeOut !");
    m_diagnoseTimer.stop();
    OnDisconnected();
}

void SocketServer::OnReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());

    if(socket == m_dataSocket )
    {
        m_dataPacket.data += socket->readAll();

        // 循环解析包数据，m_dataPacket.data 中可能不只一包数据
        bool isOk = false;
        do{
            isOk = parsePacket(socket, &m_dataPacket);
        }while(isOk);
    }
    else
    {
        qDebug()<<"socket connection abnormal";
    }


}

// 解包
bool SocketServer::parsePacket(QTcpSocket *socket, SocketPacket *packet)
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
    if(tmpPacket.data.count() < NET_PACKET_LTNGTH_BYTES)
    {
        return false;
    }
    bool isOk;
    tmpPacket.length = tmpPacket.data.mid(0, NET_PACKET_LTNGTH_BYTES).toLong(&isOk);
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
    tmpPacket.data.remove(0, NET_PACKET_LTNGTH_BYTES);//删除数据长度
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


    //发送数据包消息
    if(socket == m_dataSocket)
    {
        emit dataPacketReady(tmpPacket);
    }

    //删除当前包数据
    packet->data.remove(0,
                        QByteArray(NET_PACKET_START).size()
                        + NET_PACKET_LTNGTH_BYTES
                        + tmpPacket.length);

    return true;
}

void SocketServer::OnDisconnected()
{
    qDebug()<<"OnDisconnected";

    m_diagnoseTimer.stop();

    if(m_dataSocket != NULL)
    {
        m_dataSocket->abort();
        m_dataSocket = NULL;
    }

    emit socketConnected(false);
}

// 封包并发送
void SocketServer::send(QByteArray data, QByteArray dataType)
{
    QTcpSocket *socket = m_dataSocket;

    if(socket == NULL || (socket !=  NULL &&
       socket->state() !=  QTcpSocket::ConnectedState))
    {
        return;
    }

    QByteArray packet = dataType + data + NET_PACKET_END; //[包类型 + 数据 + 包尾]

    //长度占8字节，前面补零，如"00065536"
    int size = packet.size();
    QByteArray length = QByteArray().setNum(size);
    length = QByteArray(NET_PACKET_LTNGTH_BYTES, '0') + length;
    length = length.right(NET_PACKET_LTNGTH_BYTES);

    packet.insert(0, NET_PACKET_START + length);//插入 [包头 + 数据长度]

    socket->write(packet);
}
