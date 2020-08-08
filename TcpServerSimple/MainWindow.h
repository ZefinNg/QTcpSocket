#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Network/SocketServer.h"
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SocketServer *m_socketServer;
    NetLedControl m_netLedControl;

    void switchLedStatus(bool value, QPushButton *button);
    void showPlainText(QString text);
    void processSocketCmd(const QByteArray cmd);

private slots:
    void socketConnected(bool value);
    void dataPacketReady(SocketPacket socketData);
    void on_pushButton_send_text_clicked();
};

#endif // MAINWINDOW_H
