#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDateTime>
//#include "MyDebug.h"
#include <QMessageBox>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_socketServer = SocketServer::getInstance();
    connect(m_socketServer, SIGNAL(socketConnected(bool)),
            this, SLOT(socketConnected(bool)));
    connect(m_socketServer, SIGNAL(dataPacketReady(SocketPacket)),
            this, SLOT(dataPacketReady(SocketPacket)));

    switchLedStatus(m_netLedControl.led1, ui->pushButton_led_1);
    switchLedStatus(m_netLedControl.led2, ui->pushButton_led_2);
    switchLedStatus(m_netLedControl.led3, ui->pushButton_led_3);
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

void MainWindow::dataPacketReady(SocketPacket packet)
{
    showPlainText("Received dataType " + packet.dataType);

    if(packet.dataType == NET_PACKET_TYPE_LED_CONTROL)
    {
        NetLedControl netLedControl;
        memcpy(&netLedControl, packet.data.data(), sizeof(NetLedControl));

        m_netLedControl = netLedControl;
        switchLedStatus(m_netLedControl.led1, ui->pushButton_led_1);
        switchLedStatus(m_netLedControl.led2, ui->pushButton_led_2);
        switchLedStatus(m_netLedControl.led3, ui->pushButton_led_3);
    }
    else if(packet.dataType == NET_PACKET_TYPE_CMD)
    {
        QByteArray cmd = packet.data;
        processSocketCmd(cmd);
    }
    else if(packet.dataType == NET_PACKET_TYPE_TEXT)
    {
        QDir dir;
        QString strDir = "C:/TMP_TMP/";
        if (!dir.exists(strDir))
        {
            dir.mkpath(strDir);
        }
        QFile file("C:/TMP_TMP/head first design patterns.pdf");
        if (!file.open(QIODevice::WriteOnly))
        {
            qDebug() << __FUNCTION__ << "failed";
        }
        else {
            qDebug() << __FUNCTION__ <<  "successed";
            file.write(packet.data);
        }

//        NetText netText;
//        netText.read(packet.data);
//        showPlainText("Received text : " + netText.m_text);
    }
}

void MainWindow::processSocketCmd(const QByteArray cmd)
{
    if(NET_CMD_GET_SYS_STATUS == cmd)
    {
        NetSystemStatus netSystemStatus;
        netSystemStatus.temperature = ui->comboBox_temperature->currentText().toFloat();
        netSystemStatus.pressure    = ui->comboBox_pressure->currentText().toFloat();
        netSystemStatus.led1 = m_netLedControl.led1;
        netSystemStatus.led2 = m_netLedControl.led2;
        netSystemStatus.led3 = m_netLedControl.led3;
        QByteArray ba;
        netSystemStatus.write(&ba);
        m_socketServer->send(ba, NET_PACKET_TYPE_SYS_STATUS);
    }
}

void MainWindow::switchLedStatus(bool value, QPushButton *button)
{
    if(value)
    {
        button->setIcon(QIcon(":/Pic/LED_on.png"));
    }
    else
    {
        button->setIcon(QIcon(":/Pic/LED_off.png"));
    }
}

void MainWindow::showPlainText(QString text)
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString msg = dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz");
    msg += "\n" + text + "\n\n";
    ui->plainTextEdit->appendPlainText( msg );
}

void MainWindow::on_pushButton_send_text_clicked()
{
    QString text = ui->lineEdit_text->text();
    if(!text.isEmpty())
    {
        NetText netText;
        netText.m_text = text;
        QByteArray ba;
        netText.write(&ba);
        m_socketServer->send(ba, NET_PACKET_TYPE_TEXT);
    }
    else
    {
        QMessageBox::warning(this, "提示", "文本内容不能为空。");
    }
}
