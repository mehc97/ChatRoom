#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent ,QString name);
    //Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    //通信套接字
    QTcpServer* m_tcp;
    //端口
    qint64 port;
    //检查聊天窗口是不是开着的
    bool isopen = false;
};
#endif // WIDGET_H
