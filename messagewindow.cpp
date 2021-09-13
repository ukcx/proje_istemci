#include "messagewindow.h"
#include "ui_messagewindow.h"
#include "message.h"
#include "client_operations.h"


MessageWindow::MessageWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MessageWindow)
{
    ui->setupUi(this);

    MessageWindow::findChild<QPushButton *>("sendMessage")->setAutoDefault(false);

    QPushButton * button = MessageWindow::findChild<QPushButton *>("sendMessage");
    connect(button, SIGNAL(released()), this, SLOT(sendMessage_clicked()));
}

MessageWindow::~MessageWindow()
{
    delete ui;
}

void MessageWindow::sendMessage_clicked()
{
    to = ui->lineEdit_to->text().toUtf8().constData();
    cc = ui->lineEdit_cc->text().toUtf8().constData();
    subject = ui->lineEdit_subject->text().toUtf8().constData();
    body = ui->textEdit_body->toPlainText().toUtf8().constData();
    pLevel = ui->comboBox_pLevel->currentText().toUtf8().constData();
    placeToWrite = ui->comboBox_place->currentText().toUtf8().constData();

    Message msg(to, cc, subject, body, convertStringToPriorityLevel(pLevel));
    std::string message = "";

    if( (client->sendMessage(placeToWrite))
            && (client->sendMessage(msg.messageToString())))
    {
        if(client->recieveMessage(message))
        {
            QMessageBox::information(this, "Message Has Been Sent", "Server's feedback: " + QString::fromStdString(message));
        }
        else
        {
            QMessageBox::warning(this, "Message is sent but server could not reply back!", QString::fromStdString(message));
        }
    }
    else
    {
        QMessageBox::warning(this, "Message", "Message can not be sent!");
    }

    this->hide();

}




