#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

enum priorityLevel { Low, Medium, High };    //ekstra priorityLevel ekleyebilirsiniz
                                             //priorityLevel seviyeleri index olarak kullanilmistir
                                             //eger yeni priorityLevel eklenecekse bir degere esitlemeyin

//priorityLevel ogeleri uzerinde yapacaginiz degisiklikleri priority_map ve string_map uzerine ekleyin

std::unordered_map <priorityLevel, std::string> priority_map = {
    {Low, "Low"},
    {Medium, "Medium"},
    {High, "High"}
};

std::unordered_map <std::string, priorityLevel> string_map = {
    {"Low", Low },
    {"Medium", Medium },
    {"High", High }
};

class Message
{
public:
    Message(std::string to, std::string cc, std::string subject, std::string body,
            priorityLevel priority);

    void printMessage();                        //mesaji konsola yazdir
    void printMessage(std::ofstream&);               //mesaji verilen ofstream ogesi kullanarak yazdir
    std::string messageToString();                   //mesaji stringe cevirir
    priorityLevel getPriority();                //priority'i direkt dondur
    std::string getPriorityInfo();                   //priority'i string halinde dondur

private:
    std::string to;
    std::string cc;
    std::string subject;
    std::string body;
    priorityLevel priority;

};

//MEMBER FONKSIYONLARI**********************************************************************************************

/*
*Default Message constructor
*/
Message::Message(std::string t, std::string c, std::string subj,
                 std::string bdy, priorityLevel prio)
{
    to = t;
    cc = c;
    subject = subj;
    body = bdy;
    priority = prio;
}

/*
* Private fonskiyon, enum tipte olan priority'i string'e cevir
*/
std::string Message::getPriorityInfo()
{
    return priority_map.find(priority)->second; //second (key, value) pair'indeki value'u dondurur
}

/*
*Mesaji duzgun sekilde yazdirir.
*/
void Message::printMessage()
{
    std::cout << "To: " << this->to << std::endl;
    std::cout << "Cc: " << this->cc << std::endl;
    std::cout << "Subject: " << this->subject << std::endl;
    std::cout << "Body: " << this->body << std::endl;
    std::cout << "Priority: " << getPriorityInfo() << std::endl;
}

/*
*Mesaji duzgun sekilde verilen dosyaya yazdirir.
*/
void Message::printMessage(std::ofstream &file_descriptor)
{
    file_descriptor << "To: " << this->to << std::endl;
    file_descriptor << "Cc: " << this->cc << std::endl;
    file_descriptor << "Subject: " << this->subject << std::endl;
    file_descriptor << "Body: " << this->body << std::endl;
    file_descriptor << "Priority: " << getPriorityInfo() << std::endl;
}

/*
* Mesaji icindeki bilgileri arasina ve sonuna \n ekleyerek string'e donusturur.
*/
std::string Message::messageToString()
{
    return to + "\n" + cc + "\n" + subject + "\n" + body + "\n" + getPriorityInfo() + "\n";
}

/*
* Mesajin oncelik seviyesini geri dondurur.
*/
priorityLevel Message::getPriority()
{
    return priority;
}

//FREE FONSIYONLAR**************************************************************************************************

/*
*Bu fonksiyon bir string objesi alip onu priorityLevel objesine donusturur.
priorityLevel sinifi -- enum priorityLevel { Low, Medium, High } -- seklinde tanimlanmistir.
Eger verilen string tanimlanan herhangi bir priorityLevel seviyesi degilse hata mesaji dondurulur.
Buyuk ve kucuk harf kullanimi tolere edilmistir.
*/
priorityLevel convertStringToPriorityLevel(std::string str)   //parse
{
    if (str.length() > 0)
    {
        std::string temp = "";
        temp += toupper(str[0]);
        for (int i = 1; i < str.length(); i++)
        {
            temp += tolower(str[i]);
        }

        auto findTemp = string_map.find(temp);

        if (findTemp != string_map.end()) {
            return findTemp->second;                //second (key, value) pair'indeki value'u dondurur
        }
        else
        {
             throw "Bu gecerli bir oncelik seviyesi degil!!";
        }
    }
    else
    {
        throw "Bu gecerli bir oncelik seviyesi degil!!";
    }
}

/*
*Bu fonksiyon bir string objesi alip onu Message objesine donusturur.
Verilen string "to + \n + cc + \n + subject + \n + body + \n + priority + \n"
seklinde olusturulmus olmalidir.
messageToString() member fonksiyonunun tersidir.
*/
Message stringToMessage(std::string msg)
{
    std::string info[5] = { "", "", "", "", "" };

    int i = 0;
    for (int j = 0; (j < msg.length()) && (i < 5); j++)
    {
        if (msg[j] == '\n')
            i++;
        else
        {
            info[i] += msg[j];
        }
    }

    Message message(info[0], info[1], info[2], info[3], convertStringToPriorityLevel(info[4]));

    return message;
}
#endif
