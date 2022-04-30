#include "login.h"
#include "ui_login.h"
#include "widget.h"
#include <QMessageBox>

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);

    this->setWindowTitle("登录");
    //this->setFixedSize(482,83);




}

login::~login()
{
    delete ui;
}

void login::on_login_pushButton_clicked()
{
    if(ui->username_edit->text().isEmpty())
    {
        QMessageBox::information(this,"提示","你的用户名为空");
        return;
    }
    Widget * widget = new Widget(0,ui->username_edit->text());

    widget->show();
    this->close();
}

