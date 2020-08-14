#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>
#include "tcpsocket.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    void setupUi();

private slots:
    void onconnectTcpServer();
    void ondisconnectTcpServer();
    void onsendfile();
    void onsendtext();

    void onconnected();
    void ondisconnected();
private:
    QLabel* m_IPLabel;
    QLineEdit* m_IPLineEdit;
    QLabel* m_PortLabel;
    QSpinBox* m_PortSpinBox;

    QPushButton* m_ConnectBtn;
    QPushButton* m_DisConnectBtn;

    QPushButton* m_SendFileBtn;
    QLineEdit* m_LineEdit;
    QPushButton* m_SendTextBtn;

    QLabel* m_NetworkStatusLabel;

    TcpSocket* m_socket;
};

#endif // WIDGET_H
