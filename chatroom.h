#ifndef CHATROOM_H
#define CHATROOM_H

#include <QWidget>
#include <QTcpSocket>
#include <QString>
#include <QDateTime>
#include <QStringList>
#include <QList>

namespace Ui {
class talkroom;
}


class talkroom : public QWidget
{
    Q_OBJECT

public:
    explicit talkroom(QWidget *parent, QString name);


    //重写窗口退出事件
    void closeEvent(QCloseEvent *event);

    ~talkroom();

private slots:
    void on_outtalk_clicked();

    void on_sendmeg_clicked();

private:
    Ui::talkroom *ui;

    //通信套接字
    QTcpSocket * m_tcp;
    //ip
    QString m_ip;
    //端口
    QString m_port;
    //用户名
    QString m_name;
    //创建一个容器存储收到的用户名
    QList<QString> list;

    //存储设置的字体
    QFont m_font;
    QString m_size;

protected:
    bool eventFilter(QObject *target, QEvent *event);

signals:
    void closeWindow();
    void connServer();

};

#endif // CHATROOM_H
