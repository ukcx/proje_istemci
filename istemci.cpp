#include "message.h"
#include "client_operations.h"
#include "mainwindow.h"
#include "mainwindow.cpp"
#include "messagewindow.h"
#include "messagewindow.cpp"
#include <QApplication>

std::string server_address = "127.0.0.1";     //"localhost";
int port_num = 194;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(nullptr, server_address, port_num);
    w.show();
    return a.exec();
}
