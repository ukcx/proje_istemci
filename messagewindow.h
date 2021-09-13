#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <string>


namespace Ui {
class MessageWindow;
}

class MessageWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MessageWindow(QWidget *parent = nullptr);
    ~MessageWindow();


private slots:
    void sendMessage_clicked();

private:
    Ui::MessageWindow *ui;
    std::string to;
    std::string cc;
    std::string subject;
    std::string body;
    std::string pLevel;
    std::string placeToWrite;

};

#endif // MESSAGEWINDOW_H
