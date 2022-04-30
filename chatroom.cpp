#include "chatroom.h"
#include "ui_chatroom.h"
#include <QHostAddress>
#include <QDebug>
#include <QFontComboBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <synchapi.h>
#include <QKeyEvent>

talkroom::talkroom(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::talkroom)
{
    ui->setupUi(this);
    this->setFixedSize(843,649);
    m_name = name;

    ui->in_textEdit->setFontPointSize(14);


    m_tcp = new QTcpSocket(this);
    m_port = "xxxx";
    unsigned short port = m_port.toUShort();
    m_ip = "xxx.xxx.xx.xxx";
    m_tcp->connectToHost(QHostAddress(m_ip),port);


    //收到有数据的信号，读取服务器发过来的数据
    connect(m_tcp,&QTcpSocket::readyRead, this, [=](){
        QByteArray data = m_tcp->readAll();

        //如果返回字符串带有谁进入了聊天室，就执行下面操作

        if(data.lastIndexOf("用户：") != -1 && data.lastIndexOf("进入"))
        {
            QString msg = "用户second:" + m_name ;
            m_tcp->write(msg.toUtf8());
        }

        int one_str = data.lastIndexOf("用户：");
        int second_str = data.lastIndexOf("离开了");
        if(one_str != -1 && second_str != -1)
        {
            int str = data.lastIndexOf("：");
            QString out_name = data.right(data.size() - (str + 3));

            int out2_str = out_name.lastIndexOf("离开了");
            QString out2_name = out_name.left(out2_str);

            int i;
            for(i = 0; i < list.size(); i++)
            {
                if(list[i].compare(out2_name) == 0)
                {
                    list.removeAt(i);
                    bool isEmpty = ui->usrname_tableWidget->findItems(out2_name, Qt::MatchExactly).isEmpty();
                    if(!isEmpty)
                    {
                         //找到名字的那一行删除
                         int row = ui->usrname_tableWidget->findItems(out2_name, Qt::MatchExactly).first()->row();
                         ui->usrname_tableWidget->removeRow(row);

                         //在线人数更新
                         ui->online_usr->setText(QString("在线用户：%1人").arg(ui->usrname_tableWidget->rowCount()));

                    }

                    break;
                }
            }

        }

        //返回了带名字的字符串，就加到链表里
        int start_str = data.lastIndexOf("online_usr:");
        if(start_str != -1)
        {

             QString in_name = data.right(data.size() - (start_str + 11));

             //把用户名添加到字符数组里，如果有重复的就不添加
             int i;
             for(i = 0; i < list.size(); i++)
             {
                 if(list[i].compare(in_name) == 0)
                 {
                     return;
                 }
             }
             list.push_back(in_name.toUtf8());

             for(int i = 0; i < list.size(); i++)
             {
                 bool isEmpty = ui->usrname_tableWidget->findItems(list[i], Qt::MatchExactly).isEmpty();
                 if(isEmpty)
                 {
                     QTableWidgetItem * usr = new QTableWidgetItem(list[i]);

                     //插入行
                     ui->usrname_tableWidget->insertRow(0);
                     ui->usrname_tableWidget->setItem(0,0,usr);

                     //在线人数更新
                     ui->online_usr->setText(QString("在线用户：%1人").arg(ui->usrname_tableWidget->rowCount()));
                 }
             }

        }
        else
        {
             if(data.size() > 400)
             {
                //找出名字和时间
                int name_pos = data.lastIndexOf("</html>");

                //把名字时间放前面拼接
                QString name = data.right(data.size() - (name_pos +7 ));

                data.remove(name_pos,50);

                ui->out_textBrowser->setTextColor(Qt::blue);
                ui->out_textBrowser->append((name));

                ui->out_textBrowser->setTextColor(Qt::black);
                ui->out_textBrowser->append(QString(data));
            }
            else
            {
                ui->out_textBrowser->append(QString(data));
            }
        }

    });


    //收到客户端断开连接的信号，关闭连接
    connect(m_tcp, &QTcpSocket::disconnected, this, [=](){

//        QMessageBox::warning(this,"警告","你已和服务器断开连接");


    });


    //收到连接成功的信号
    connect(m_tcp, &QTcpSocket::connected, this, [=](){


        //设置标签已连接
        ui->isconn_label->setText(QString("已连接服务器"));

        //输出用户加入房间的信息
        QString msg = "用户：" + m_name +  "进入了聊天室";
        m_tcp->write(msg.toUtf8());

        list.push_back(m_name);

        //查看用户名在不在列表里
        for(int i = 0; i < list.size(); i++)
        {
            bool isEmpty = ui->usrname_tableWidget->findItems(list[i], Qt::MatchExactly).isEmpty();
            if(isEmpty)
            {
                QTableWidgetItem * usr = new QTableWidgetItem(list[i]);

                //插入行
                ui->usrname_tableWidget->insertRow(0);
                ui->usrname_tableWidget->setItem(0,0,usr);
                //追加聊天记录
                //ui->out_textBrowser->setTextColor(Qt::gray);
                //ui->out_textBrowser->append(QString(m_name + "进入了聊天室0.0"));

                //在线人数更新
                ui->online_usr->setText(QString("在线用户：%1人").arg(ui->usrname_tableWidget->rowCount()));
            }

        }

    });


    //---------------辅助功能----------------//
    //改变字体
    connect(ui->wordtype, &QFontComboBox::currentFontChanged, this, [=](const QFont &font){
        ui->in_textEdit->setCurrentFont(font);
        //ui->in_textEdit->setFontPointSize(14);
        ui->in_textEdit->setFontPointSize(ui->wordsize->currentText().toDouble());

        this->m_font = font;
        qDebug()<<this->m_font;

        ui->in_textEdit->setFocus();
    });
    //改变字号
    void (QComboBox:: *cbxsingnal)(const QString &text) = &QComboBox::currentTextChanged;
    connect(ui->wordsize, cbxsingnal, this, [=](const QString &text){
       ui->in_textEdit->setFontPointSize((text.toDouble()));

       this->m_size = text;
       qDebug()<<this->m_size;

       ui->in_textEdit->setFocus();

    });
    //加粗字体
    connect(ui->wordbold, &QToolButton::clicked, this, [=](bool ischeck)
    {
       if(ischeck)
       {
           ui->in_textEdit->setFontWeight(QFont::Bold);
       }
       else
       {
           ui->in_textEdit->setFontWeight(QFont::Normal);
       }
    });
    //添加倾斜
    connect(ui->wordlics, &QToolButton::clicked, this, [=](bool ischeck)
    {
        ui->in_textEdit->setFontItalic(ischeck);
    });
    //添加下划线
    connect(ui->wordline, &QToolButton::clicked, this, [=](bool ischeck)
    {
       ui->in_textEdit->setFontUnderline(ischeck);
    });
    //字体颜色
    connect(ui->wordcolor, &QToolButton::clicked, this, [=](){
       QColor color = QColorDialog::getColor(Qt::black);
       ui->in_textEdit->setTextColor(color);
    });
    //保存聊天记录
    connect(ui->savetalk, &QToolButton::clicked, this, [=](){
        if(ui->out_textBrowser->document()->isEmpty())
        {
            QMessageBox::warning(this,"警告","不能保存空的聊天记录！");
        }


        else
        {
            QString path = QFileDialog::getSaveFileName(this,"保存聊天","聊天记录","(*.txt)");
//            if(path.isEmpty())
//            {
//                QMessageBox::warning(this,"警告","保存位置不能为空！");
//            }
            QFile file(path);
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream stream(&file);
            //QString str = ui->out_textBrowser->toPlainText();
            stream<<ui->out_textBrowser->toPlainText();
            file.close();
        }
     });
     //清屏
     connect(ui->cleartalk, &QToolButton::clicked, this, [=](){
        ui->out_textBrowser->clear();
     });

     //绑定输入框的状态，如果输入框为空，设置文本的字体字号
     connect(ui->in_textEdit, &QTextEdit::textChanged, [=](){
         if(ui->in_textEdit->toPlainText().isEmpty())
         {
            qDebug()<<"ok";
   //       ui->in_textEdit->setFontPointSize(this->m_size.toDouble());
            ui->in_textEdit->setCurrentFont(ui->wordtype->currentFont());
            ui->in_textEdit->setFontPointSize(ui->wordsize->currentText().toDouble());
         }

     });

     //如果收到通信描述符的错误信号
     connect(m_tcp, &QAbstractSocket::errorOccurred, this,[=](){

         if(m_tcp->error() == QAbstractSocket::RemoteHostClosedError)
         {
             QMessageBox::warning(this,"警告","你已和服务器断开连接");
             ui->isconn_label->setText(QString("未连接服务器"));
         }
     });

     //绑定回车发送信息
     ui->sendmeg->setFocus();
     ui->sendmeg->setDefault(true);
     ui->in_textEdit->installEventFilter(this);


 }

 void talkroom::closeEvent(QCloseEvent *e)
 {

     //追加离开聊天记录
     QString out_msg = "用户：" + m_name + "离开了聊天室";
     m_tcp->write(out_msg.toUtf8());

     Sleep(100);

     //关闭套接字
     m_tcp->close();
     m_tcp->destroyed();
     //talkroom::closeEvent(e);
     m_tcp->deleteLater();
     //发送关闭信号
     emit this->closeWindow();


 }

 talkroom::~talkroom()
 {
     delete ui;
 }

 void talkroom::on_outtalk_clicked()
 {

     //追加离开聊天记录
     QString out_msg = "用户：" + m_name + "离开了聊天室";
     m_tcp->write(out_msg.toUtf8());

     Sleep(100);

     m_tcp->close();
     this->close();

 }

 void talkroom::on_sendmeg_clicked()
 {
     //如果输入框为空，不发送
     if(ui->in_textEdit->toPlainText().isEmpty())
     {
         return;
     }
     //获取当前时间
     QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    //把发送者和时间拼在一起
     QString name_msg = "[ " + m_name + " ]" + time;
   //  m_tcp->write(name_msg.toUtf8());
    //把上面的字符串和发送内容拼一起
     QString in_msg = ui->in_textEdit->toHtml() + name_msg;

     //往服务器写入数据
     m_tcp->write(in_msg.toUtf8());

     //清空输入框，鼠标焦点移到输入框
     ui->in_textEdit->clear();
     ui->in_textEdit->setFocus();
 }

 //实现按下回车键发送信息
 bool talkroom::eventFilter(QObject *target, QEvent *event)
 {
     if(target == ui->in_textEdit)
     {
         if(event->type() == QEvent::KeyPress)
         {
             QKeyEvent *k = static_cast<QKeyEvent *>(event);
             if(k->key() == Qt::Key_Return)
             {
                 this->on_sendmeg_clicked();
                 return true;
             }
//             if((k->modifiers() == Qt::ControlModifier) && (k->key() == Qt::Key_B))
//             {
//                ui->in_textEdit->append(" \n");
//                return true;
//             }

         }
     }
     return QWidget::eventFilter(target,event);
 }



