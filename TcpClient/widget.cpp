#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    connect(m_ConnectBtn, &QPushButton::clicked, this, &Widget::onconnectTcpServer);
    connect(m_DisConnectBtn, &QPushButton::clicked, this, &Widget::ondisconnectTcpServer);
    connect(m_SendFileBtn, &QPushButton::clicked, this, &Widget::onsendfile);
    connect(m_SendTextBtn, &QPushButton::clicked, this, &Widget::onsendtext);

    m_socket = new TcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &Widget::onconnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &Widget::ondisconnected);
}

Widget::~Widget()
{

}

void Widget::setupUi()
{
    /************ widget ****************/
    m_IPLabel = new QLabel("IP 地址", this);
    m_IPLineEdit = new QLineEdit("127.0.0.1", this);
    m_PortLabel = new QLabel("端口号", this);
    m_PortSpinBox = new QSpinBox(this);
    m_PortSpinBox->setMaximum(65535);
    m_PortSpinBox->setValue(static_cast<int>(port));

    m_ConnectBtn = new QPushButton("连接", this);
    m_DisConnectBtn = new QPushButton("断开连接", this);
    m_DisConnectBtn->setEnabled(false);

    m_SendFileBtn = new QPushButton("发送文件", this);
    m_LineEdit = new QLineEdit(this);
    m_SendTextBtn = new QPushButton("发送文本", this);

    m_NetworkStatusLabel = new QLabel(this);
    QString text = tr("连接状态：");
    text += QString("<font color=red>%1</font>").arg(tr("未连接"));
    m_NetworkStatusLabel->setText(text);

    /************ layout ****************/
    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addWidget(m_IPLabel);
    topLayout->addWidget(m_IPLineEdit);
    topLayout->addWidget(m_PortLabel);
    topLayout->addWidget(m_PortSpinBox);

    QHBoxLayout* top2Layout = new QHBoxLayout;
    top2Layout->addWidget(m_ConnectBtn);
    top2Layout->addWidget(m_DisConnectBtn);

    QHBoxLayout* midLayout = new QHBoxLayout;
    midLayout->addWidget(m_LineEdit);
    midLayout->addWidget(m_SendTextBtn);

    QHBoxLayout* btmLayout = new QHBoxLayout;
    btmLayout->addWidget(m_NetworkStatusLabel);
    btmLayout->addStretch();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(topLayout);
    layout->addLayout(top2Layout);
    layout->addWidget(m_SendFileBtn);
    layout->addLayout(midLayout);
    layout->addLayout(btmLayout);
}

void Widget::onconnectTcpServer()
{
    m_socket->connectTcpServer(m_IPLineEdit->text(), static_cast<quint16>(m_PortSpinBox->value()));
}

void Widget::ondisconnectTcpServer()
{
    m_socket->disconnectFromHost();
}

void Widget::onsendfile()
{
    QString filepath = QFileDialog::getOpenFileName(this, "Send file");
    if (filepath.isEmpty())
        return;
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QFileInfo info(filepath);
    QString filename = info.fileName();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << filename << file.readAll();
    file.close();
    m_socket->send(block, NET_PACKET_TYPE_SEND_FILE);
}

void Widget::onsendtext()
{
    QString text = m_LineEdit->text();
    if (!text.isEmpty()) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out << text;
        m_socket->send(block, NET_PACKET_TYPE_SEND_TEXT);
    }
}

void Widget::onconnected()
{
    QString text = tr("连接状态：");
    text += QString("<font color=blue>%1</font>").arg(tr("已连接"));
    m_NetworkStatusLabel->setText(text);
    m_ConnectBtn->setEnabled(false);
    m_DisConnectBtn->setEnabled(true);
}

void Widget::ondisconnected()
{
    QString text = tr("连接状态：");
    text += QString("<font color=red>%1</font>").arg(tr("未连接"));
    m_NetworkStatusLabel->setText(text);
    m_ConnectBtn->setEnabled(true);
    m_DisConnectBtn->setEnabled(false);
}
