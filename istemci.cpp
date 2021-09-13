#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS         //inet_addr kullanmak icin
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#pragma comment(lib, "Ws2_32.lib")              //winsock2'yi linkleyebilmek icin
#include <iostream>
#include <string.h>
#include "message.h"
#include <winsock2.h>

using namespace std;

constexpr auto MSG_SIZE = 1000;
string sunucu_adresi = "127.0.0.1";     //"localhost";
int sunucu_portu = 194;

int main()
{
    WSADATA wsa;            //winsock kutuphanesini baslatmak icin
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Winsock baslatilamadi. Error Code : %d", WSAGetLastError());
        return 1;
    }

    sockaddr_in addr;                   //soket icin gerekli adres bilgileri
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;          //IPv4 
    addr.sin_addr.s_addr = inet_addr(sunucu_adresi.c_str());   //sunucu IP adresi
    addr.sin_port = htons(sunucu_portu);

    SOCKET soket = socket(AF_INET, SOCK_STREAM, 0);     //SOCK_STREAM TCP/IP protokolune uygun olan sabit

    if (soket == INVALID_SOCKET)
    {
        cerr << "soket olusturulamadi\n";
        return 1;
    }
    else
    {
        //cout << "soket numarasi: " << soket << endl;
        if (connect(soket, (sockaddr*)&addr, sizeof(addr)) < 0)    //sunucuya baglan
        {
            cerr << "sunucuya baglanma hatasi" << endl;
            return 1;
        }
        else
        {
            cout << "sunucuya baglanildi" << endl << endl;

            string cc, to, subject, body, prio;

            cout << "Mesaj bilgilerinizi asagiya giriniz:" << endl;
            cout << "To: ";
            getline(cin, to);                       //getline ile bosluk karakteri de alinabilir
            cout << "Cc: ";
            getline(cin, cc);
            cout << "Subject: ";
            getline(cin, subject);
            cout << "Body: ";
            getline(cin, body);
            cout << "Priority Level (low, medium, high): ";
            cin >> prio;

            try
            {
                stringToEnum(prio);           //oncelik seviyesi dogru girilmediyse mesaj olusturmuyoruz
            }
            catch (const char* error_message)
            {
                cerr << error_message << endl;
                return 1;
            }

            Message* mesaj = new Message(to, cc, subject, body, stringToEnum(prio));

            if (send(soket, mesaj->messageToString().c_str(), mesaj->messageToString().length(), 0) < 0)   //write, send ile soket uzerine mesaj yaziyoruz
            {
                cerr << "\nMesaj gonderilemedi" << endl;
                return 1;
            }
            else
            {
                cout << "\nMesaj gonderildi" << endl;
                cout << "\nGonderilen mesaj: " << endl;
                mesaj->printMessage();
                delete mesaj;

                char alinan_mesaj[MSG_SIZE];
                memset(&alinan_mesaj, 0, MSG_SIZE);
                recv(soket, alinan_mesaj, MSG_SIZE, 0); //read, recv ile soket uzerinden mesaji okuyoruz
                cout << "\nMesaj alindi" << endl;

                mesaj = stringToMessage(alinan_mesaj);
                cout << "\nAlinan mesaj: " << endl;
                mesaj->printMessage();


            }
            delete mesaj;

        }
    }
    closesocket(soket);
    return 0;
}