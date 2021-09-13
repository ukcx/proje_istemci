#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS         //inet_addr kullanmak icin
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#pragma comment(lib, "Ws2_32.lib")              //winsock2'yi linkleyebilmek icin
#include <iostream>
#include <string.h>
#include <winsock2.h>
#include <conio.h>                              //_khbit() ve _getch() fonksiyonlari icin
#include "message.h"

using namespace std;

constexpr auto MSG_SIZE = 1000;
string sunucu_adresi = "127.0.0.1";     //"localhost";
int sunucu_portu = 194;

int main()
{
    WSADATA wsa;            //winsock kutuphanesini baslatmak icin
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Winsock baslatilamadi. Hata kodu: %d", WSAGetLastError());
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
        cerr << "Soket olusturulamadi\n";
        return 1;
    }
    else
    {
        if (connect(soket, (sockaddr*)&addr, sizeof(addr)) < 0)    //sunucuya baglan
        {
            cerr << "Sunucuya baglanma hatasi" << endl;
            return 1;
        }
        else
        {
            cout << "Sunucuya baglanildi" << endl;

            string cc, to, subject, body, prio;
            string garbage_collector;
            bool isInputInCorrectFormat = true;
            char escapeCharacter = '\n';

            while (escapeCharacter != 27)       //ESC karakteri girilene kadar mesaj girilmeye devam ediliyor
            {
                isInputInCorrectFormat = true;

                cout << endl << "Mesaj bilgilerinizi asagiya giriniz:" << endl;
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

                getline(cin, garbage_collector);    //eger cin'de extra mesaj kaldiysa topla

                try         //oncelik seviyesi dogru girilmediyse mesaj olusturmuyoruz
                {
                    convertStringToPriorityLevel(prio);
                }
                catch (const char* error_message)
                {
                    cerr << endl << error_message << endl;
                    cerr << "Mesajiniz gonderilemedi" << endl;
                    isInputInCorrectFormat = false;    //mesajin oncelik seviyesi yanlis, mesaj olusturulamaz 
                }

                if (isInputInCorrectFormat)
                {
                    Message* mesaj = new Message(to, cc, subject, body, convertStringToPriorityLevel(prio));

                    if (send(soket, mesaj->messageToString().c_str(), mesaj->messageToString().length(), 0) < 0)   //write, send ile soket uzerine mesaj yaziyoruz
                    {
                        cerr << "\nMesaj gonderilemedi" << endl;
                        return 1;
                    }
                    else    //mesaj basariyla gonderildi
                    {
                        cout << "\nMesaj gonderildi" << endl;
                        cout << "\nGonderilen mesaj: " << endl;
                        mesaj->printMessage();
                        delete mesaj;

                        char alinan_mesaj[MSG_SIZE];
                        memset(&alinan_mesaj, 0, MSG_SIZE);
                        recv(soket, alinan_mesaj, MSG_SIZE, 0); //read, recv ile soket uzerinden mesaji okuyoruz
                        cout << "\nSunucudan alinan mesaj: " << alinan_mesaj << endl;
                    }
                }

                cout << "\nMesaj gonderemeye devam etmek icin Enter'a, mesaj gonderim islemini sonlandirmak icin ESC'ye basiniz!" << endl;

                string stop_message = "stop";   //kullanici ESC'ye basarsa bu mesaj sunucuya gidecek
                bool isAValidKeyPressed = false;    //kullanici sadece Enter veya ESC'ye basabilir

                while (!isAValidKeyPressed)
                {
                    if (_kbhit())   //klavyede bir tusa basildiysa true dondurur
                    {
                        int ch = 0 | _getch();  //getch() ile basilan karakteri al
                        if (ch == 0xe0 && _kbhit()) { // extended karakter (0xe0, xxx)
                            ch = (ch << 8) | _getch(); // extended karakter bilgisini al
                        }
                        switch (ch)
                        {
                        case 0x0d:      //Enter'a basildiysa loop'tan cik ve tekrar mesaj al.
                            cout << "\nEnter'a basildi" << endl;
                            isAValidKeyPressed = true;
                            break;

                        case 0x1b:      //ESC'ye basildiysa loop'tan cik ve tekrar mesaj alma. 
                            cout << "\nESC'ye basildi" << endl;
                            escapeCharacter = 27;
                            send(soket, stop_message.c_str(), stop_message.length(), 0);
                            isAValidKeyPressed = true;
                            break;

                        default:   //diger durumlarda gecerli bir karakter (enter veya esc) girilene kadar karakter almaya devam et
                            break;
                        }
                    }
                }

            }
        }
    }
    closesocket(soket);
    return 0;
}