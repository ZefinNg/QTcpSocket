#include <QCoreApplication>
#include <QDir>
#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qRegisterMetaType<qintptr>();
    QDir dir(dirPath);
    if (!dir.exists()) {
        QDir tmpdir;
        if (!tmpdir.mkpath(dirPath)) {
            qDebug() << "创建目录失败";
        }
    }
    TcpServer server;
    bool res = server.listen(QHostAddress::Any, port);
    if (res) {
        qDebug() << "监听成功";
    }
    else {
        qDebug() << "监听失败";
    }

    return a.exec();
}
