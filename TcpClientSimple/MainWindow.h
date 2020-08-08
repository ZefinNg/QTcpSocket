#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Network/SocketClient.h"

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
    SocketClient *m_socketClient;

    void ledControl();
    void showPlainText(QString text);

private slots:
    void socketConnected(bool value);
    void on_pushButton_connect_clicked();
    void on_checkBox_led_control_1_clicked();
    void on_checkBox_led_control_2_clicked();
    void on_checkBox_led_control_3_clicked();
    void on_pushButton_get_sys_status_clicked();
    void dataPacketReady(SocketPacket packet);
    void on_pushButton_send_text_clicked();
};

#endif // MAINWINDOW_H
