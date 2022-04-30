#include "widget.h"
#include "login.h"
#include <QApplication>
#include "chatroom.h"
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Widget w;
//    w.show();

    a.setStyle(QStyleFactory::create("fusion"));

    login *lwin = new login;
    lwin->show();


    return a.exec();
}
