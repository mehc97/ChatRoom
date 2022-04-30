#include "widget.h"
#include "ui_widget.h"
#include <QToolButton>
#include <QVector>
#include "chatroom.h"
#include <QString>
#include <synchapi.h>

Widget::Widget(QWidget *parent, QString name)
//Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->usrname_show->setText(name);


    //设置头像图标
    ui->head_toolButton->setIcon(QPixmap(":/images/head.png"));

    //设置软件图标
    setWindowIcon(QPixmap(":/images/icon.png"));

    //设置聊天室图标文字
    ui->toolButton_talkbutton->setText("聊天室");
    //设置聊天室图标icon
    ui->toolButton_talkbutton->setIcon(QPixmap(":/images/talkroom.png"));
    //设置icon大小
    QSize iconsize(100,200);
    ui->toolButton_talkbutton->setIconSize(iconsize);

    //创建一个容器保存按钮,方便以后管理
    QVector <QToolButton *> vToolbtn;
    vToolbtn.push_back(ui->toolButton_talkbutton);

    //点击聊天室进入聊天窗口,把用户名传到窗口
    for(int i = 0; i < vToolbtn.size(); i++)
    {
        connect(vToolbtn[i], &QToolButton::clicked, this, [=](){
           if(isopen)
           {
               return;
           }

           isopen = true;
           talkroom * room = new talkroom(0,ui->usrname_show->text());
           room->setWindowIcon(vToolbtn[i]->icon());
           Sleep(100);
           room->show();

           connect(room, &talkroom::closeWindow, this, [=](){
              isopen = false;
           });



        });
    }

}

Widget::~Widget()
{
    delete ui;
}

