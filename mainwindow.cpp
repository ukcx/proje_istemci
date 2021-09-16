#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client_operations.h"
#include <QStyle>

ClientOperations * client;

MainWindow::MainWindow(QWidget *parent, std::string ip_address, int portNum)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    server_ip_address = ip_address;
    port_number = portNum;

    ui->setupUi(this);
    client = nullptr;

    MainWindow::findChild<QPushButton *>("pushButton")->setAutoDefault(false);
    this->setCentralWidget(ui->groupBox);

    ui->lineEdit_ip->setText( QString::fromStdString(server_ip_address));
    ui->lineEdit_port->setText(QString::fromStdString(std::to_string(port_number)));
    ui->lineEdit_ip->setReadOnly(true);
    ui->lineEdit_port->setReadOnly(true);

    changeLineEditorStyle(false);

    QPushButton * button = MainWindow::findChild<QPushButton *>("pushButton");
    connect(button, SIGNAL(released()), this, SLOT(connectionButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete client;
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
   if(ui->checkBox->isChecked())
   {
       ui->lineEdit_ip->setReadOnly(false);
       ui->lineEdit_port->setReadOnly(false);

       changeLineEditorStyle(true);
   }
   else
   {
       ui->lineEdit_ip->setReadOnly(true);
       ui->lineEdit_port->setReadOnly(true);

       changeLineEditorStyle(false);
   }

}

void MainWindow::connectionButton_clicked()
{
    server_ip_address = ui->lineEdit_ip->text().toUtf8().constData();
    std::string portNum = ui->lineEdit_port->text().toUtf8().constData();
    port_number = std::atoi(portNum.c_str());

    if(client != nullptr){
        delete client;
    }
    client = new ClientOperations;
    client->initializeSocket(server_ip_address, port_number);

    if(client->connectToServer())
    {
        message_window = new MessageWindow(this);
        message_window->show();
        message_window->exec();
        delete message_window;
    }
    else
    {
        QMessageBox::warning(this, "Error", "Can not connect to the server!");
    }
}

void MainWindow::changeLineEditorStyle(bool checkbox_is_checked)
{
    ui->lineEdit_ip->setProperty("urgent", checkbox_is_checked);
    ui->lineEdit_ip->style()->unpolish(ui->lineEdit_ip);
    ui->lineEdit_ip->style()->polish(ui->lineEdit_ip);


    ui->lineEdit_port->setProperty("urgent", checkbox_is_checked);
    ui->lineEdit_port->style()->unpolish(ui->lineEdit_port);
    ui->lineEdit_port->style()->polish(ui->lineEdit_port);
}
