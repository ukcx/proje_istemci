#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <string>
#include "messagewindow.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, std::string ip_address = "127.0.0.1", int portNum = 0);
    ~MainWindow();

private slots:
    void on_checkBox_stateChanged(int arg1);

    void connectionButton_clicked();

private:
    Ui::MainWindow *ui;
    MessageWindow * message_window;
    std::string server_ip_address;
    int port_number;

    void changeLineEditorStyle(bool checkbox_is_checked);

};

#endif // MAINWINDOW_H
