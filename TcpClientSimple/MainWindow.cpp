#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_socketClient = SocketClient::getInstance();
    connect(m_socketClient, SIGNAL(socketConnected(bool)),
            this, SLOT(socketConnected(bool)));
    connect(m_socketClient, SIGNAL(dataPacketReady(SocketPacket)),
            this, SLOT(dataPacketReady(SocketPacket)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::socketConnected(bool value)
{
    QString text = tr("连接状态：");
    if(value == true)
    {
        text += QString("<font color=blue>%1</font>").arg(tr("已连接"));
    }
    else
    {
        text += QString("<font color=red>%1</font>").arg(tr("未连接"));
    }
    ui->label_network_status->setText(text);
}



void MainWindow::on_pushButton_connect_clicked()
{
    bool connected = m_socketClient->isConnected();
    if(!connected)
    {
        QHostAddress ip(ui->lineEdit_ip->text());
        int port = ui->spinBox_port->value();
        m_socketClient->connectTcpServer(ip, port);
    }
    else
    {
        QMessageBox::information(this, "提示", "已连接。");
    }
}

void MainWindow::on_checkBox_led_control_1_clicked()
{
    ledControl();
}

void MainWindow::on_checkBox_led_control_2_clicked()
{
    ledControl();
}

void MainWindow::on_checkBox_led_control_3_clicked()
{
    ledControl();
}

void MainWindow::ledControl()
{
    showPlainText("控制 LED 灯");
    NetLedControl netLedControl;
    netLedControl.led1 = ui->checkBox_led_control_1->isChecked();
    netLedControl.led2 = ui->checkBox_led_control_2->isChecked();
    netLedControl.led3 = ui->checkBox_led_control_3->isChecked();

    QByteArray data((char *)&netLedControl, sizeof(NetLedControl));
    m_socketClient->send(data, NET_PACKET_TYPE_LED_CONTROL);
}

void MainWindow::on_pushButton_get_sys_status_clicked()
{
    showPlainText("发送获取系统状态命令");
    QByteArray cmd = NET_CMD_GET_SYS_STATUS;
    m_socketClient->send(cmd, NET_PACKET_TYPE_CMD);
}


void MainWindow::showPlainText(QString text)
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString msg = dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz");
    msg += "\n" + text + "\n\n";
    ui->plainTextEdit->appendPlainText( msg );
}


void MainWindow::dataPacketReady(SocketPacket packet)
{
    if(packet.dataType == NET_PACKET_TYPE_SYS_STATUS)
    {
        NetSystemStatus netSystemStatus;
        netSystemStatus.read(packet.data);
        QString text = QString("温度：%1，压力：%2， LED1~3：%3 %4 %5")
                                .arg(netSystemStatus.temperature)
                                .arg(netSystemStatus.pressure)
                                .arg(netSystemStatus.led1)
                                .arg(netSystemStatus.led2)
                                .arg(netSystemStatus.led3);
        showPlainText(text);
    }
    else if(packet.dataType == NET_PACKET_TYPE_TEXT)
    {
        NetText netText;
        netText.read(packet.data);
        showPlainText("Received text : " + netText.m_text);
    }
}

void MainWindow::on_pushButton_send_text_clicked()
{
    QFile file("F:\\head first design patterns.pdf");
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    QByteArray ba = file.readAll();
    m_socketClient->send(ba, NET_PACKET_TYPE_TEXT);

//    QString text = ui->lineEdit_text->text();
//    if(!text.isEmpty())
//    {
//        NetText netText;
//        netText.m_text = text;
//        QByteArray ba;
//        netText.write(&ba);
//        m_socketClient->send(ba, NET_PACKET_TYPE_TEXT);
//    }
//    else
//    {
//        QMessageBox::warning(this, "提示", "文本内容不能为空。");
//    }
}
