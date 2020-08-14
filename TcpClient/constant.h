#ifndef CONSTANT_H
#define CONSTANT_H

#include <QByteArray>
#include <QDataStream>

static const QString dirPath = "C:/TMP_TMP";
static const quint16 port    = 6000;

static const long INIT_NUMBER = -1;

//包数据结构 : [ 包头 + 长度(8字节) + 包类型(2字节) + 数据 + 包尾 ]
static const char* NET_PACKET_START      =          "PACKET_START";
static const char* NET_PACKET_END        =          "PACKET_END";
static const int NET_PACKET_TYPE_BYTES   =          2;    //包类型占几个字节
static const int NET_PACKET_LENGTH_BYTES =          8;    //包数据长度占几个字节

//包类型
static const char* NET_PACKET_TYPE_SEND_FILE =      "01"; //发送文件
static const char* NET_PACKET_TYPE_SEND_TEXT =      "02"; //发送文本

//用于存储接收到的数据包
class SocketPacket
{
public:
    long       length;   //数据长度
    QByteArray data;     //数据
    QByteArray dataType; //数据类型

    SocketPacket()
    {
        length = INIT_NUMBER;
    }

    void clear()
    {
        length = INIT_NUMBER;
        data.clear();
        dataType.clear();
    }
};

struct NetSendFile
{
    QString m_filename;
    QByteArray m_bytearray;
    friend QDataStream& operator>>(QDataStream& in, NetSendFile& stru)
    {
        in >> stru.m_filename >> stru.m_bytearray;
        return in;
    }
    friend QDataStream& operator<<(QDataStream& out, const NetSendFile& stru)
    {
        out << stru.m_filename << stru.m_bytearray;
        return out;
    }
};

class NetSendText
{
public:
    QString m_text;
    friend QDataStream& operator>>(QDataStream& in, NetSendText& stru)
    {
        in >> stru.m_text;
        return in;
    }
    friend QDataStream& operator<<(QDataStream& out, const NetSendText& stru)
    {
        out << stru.m_text;
        return out;
    }
};

#endif // CONSTANT_H
