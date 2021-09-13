#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS         //inet_addr kullanmak icin
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#pragma comment(lib, "Ws2_32.lib")              //winsock2'yi linkleyebilmek icin
#include <iostream>
#include <string.h>
#include <winsock2.h>
#include <conio.h>                              //_kbhit() ve _getch() fonksiyonlari icin
#include <unordered_set>
#include "message.h"

using namespace std;

constexpr auto MSG_SIZE = 1000;
string sunucu_adresi = "127.0.0.1";     //"localhost";
int sunucu_portu = 194;
unordered_set<string> mesajin_yazdirilinabilecegi_yerler = { "console", "database", "file" };


string convertToLowerCase(string str)
{
    string lowerCaseStr = "";
    for (int i = 0; i < str.length(); i++)
    {
        lowerCaseStr += tolower(str[i]);
    }
    return lowerCaseStr;
}

int main()
{
    try
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //winsock kutuphanesini baslatmak icin gerekli
        {
            throw "Winsock baslatilamadi.";     //throw komutu yurutulurse ait olunan try scope'undan cikilir ve direkt
                                                // olarak try scope'unun disinda scope'a en yakin catch fonksiyonuna gidilir
        }

        sockaddr_in addr;                   //soket icin gerekli adres bilgileri
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;          //IPv4 
        addr.sin_addr.s_addr = inet_addr(sunucu_adresi.c_str());   //sunucu IP adresi
        addr.sin_port = htons(sunucu_portu);

        SOCKET soket = socket(AF_INET, SOCK_STREAM, 0);     //SOCK_STREAM TCP/IP protokolune uygun olan sabit

        if (soket == INVALID_SOCKET)
        {
            throw "Soket olusturulamadi";
        }


        if (connect(soket, (sockaddr*)&addr, sizeof(addr)) < 0)    //sunucuya baglan
        {
            throw "Sunucuya baglanma hatasi";
        }

        cout << "Sunucuya baglanildi" << endl;  //kodda buraya kadar herhangi bir throw komutu yürütülmediyse sunucuya basariyla baglanildi

        string cc, to, subject, body, prio;
        string garbage_collector;
        bool isInputInCorrectFormat = true;
        bool escapeCharacterIsPressed = false;      //ESC'ye basilmadi

        while (!escapeCharacterIsPressed)       //ESC karakteri girilene kadar mesaj girilmeye devam ediliyor
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

            if (isInputInCorrectFormat)             //oncelik seviyesi dogru girilmis
            {
                Message* mesaj = new Message(to, cc, subject, body, convertStringToPriorityLevel(prio));

                bool gecerli_yazdirilacak_yer = true;
                string yazdirilacak_yer;    //database, file veya console'a yazdirim icin kullanicidan input aliniyor

                cout << "Please enter where do you want your message to be written (database, file, console): ";
                getline(cin, yazdirilacak_yer);
                yazdirilacak_yer = convertToLowerCase(yazdirilacak_yer);

                try
                {
                    auto arayici = mesajin_yazdirilinabilecegi_yerler.find(yazdirilacak_yer);
                    if (arayici == mesajin_yazdirilinabilecegi_yerler.end())    //kullanicinin girdigi yazdirma yeri gecerli degil
                    {
                        throw "Gecerli yazdirilacak yer verilmedi!";
                    }
                }
                catch (const char* error_message)
                {
                    cout << endl << error_message << endl;
                    cerr << "Mesajiniz gonderilemedi" << endl;
                    gecerli_yazdirilacak_yer = false;
                }

                if (yazdirilacak_yer == "console")      //o zaman mesaji gondermeden sadece sunucuda yazdir
                {
                    cout << endl << "Mesajiniz: " << endl;
                    mesaj->printMessage();
                }
                else if (gecerli_yazdirilacak_yer)
                {
                    if (send(soket, yazdirilacak_yer.c_str(), yazdirilacak_yer.length(), 0) < 0) //write, send ile soket uzerine mesaji yazdrimak istedigimiz yeri yaziyoruz
                    {
                        cerr << "\nMesaj gonderilemedi" << endl;
                        return 1;           //eger mesaj gonderemezsek sunucunun haberi olmasi icin istemciyi kapat
                    }
                    else
                    {
                        if (send(soket, mesaj->messageToString().c_str(), mesaj->messageToString().length(), 0) < 0)   //write, send ile soket uzerine mesaj yaziyoruz
                        {
                            cerr << "\nMesaj gonderilemedi" << endl;
                            return 1;       //eger mesaj gonderemezsek sunucunun haberi olmasi icin istemciyi kapat
                        }
                        else    //mesaj basariyla gonderildi
                        {
                            cout << "\nMesaj gonderildi" << endl;

                            char alinan_mesaj[MSG_SIZE];
                            memset(&alinan_mesaj, 0, MSG_SIZE);
                            recv(soket, alinan_mesaj, MSG_SIZE, 0); //read, recv ile soket uzerinden mesaji okuyoruz
                            cout << "\nSunucudan alinan mesaj: " << alinan_mesaj << endl;
                        }
                    }
                }

                delete mesaj;
            }

            cout << "\nMesaj gondermeye devam etmek icin Enter'a, mesaj gonderim islemini sonlandirmak icin ESC'ye basiniz!" << endl;


            string stop_message = "stop";           //kullanici ESC'ye basarsa bu mesaj sunucuya gidecek
            bool isAValidKeyPressed = false;        //kullanicinin gecerli bir karaktere basip basmadigini tut (Enter veya ESC)

            while (!isAValidKeyPressed)             //Enter veya ESC basilana kadar
            {
                if (_kbhit())                       //klavyede bir tusa basildiysa true dondurur
                {
                    int ch = 0 | _getch();          //getch() ile klavyede basilan karakteri al, char 8 bit oldugu icin ch'nin 32 bitinin bastaki 24 bitini 0 yap

                    if (ch == 0xe0 && _kbhit())     // extended karakter (0xe0, xxx)
                    {
                        ch = (ch << 8) | _getch();  // extended karakteri donustur
                    }
                    switch (ch)
                    {
                    case 0x0d:      //Enter'a basildiysa loop'tan cik ve tekrar mesaj al.

                        cout << "\nEnter'a basildi" << endl;
                        isAValidKeyPressed = true;
                        break;

                    case 0x1b:      //ESC'ye basildiysa loop'tan cik ve tekrar mesaj alma. 

                        cout << "\nESC'ye basildi" << endl;
                        send(soket, stop_message.c_str(), stop_message.length(), 0);
                        escapeCharacterIsPressed = true;
                        isAValidKeyPressed = true;
                        break;

                    default:   //diger durumlarda gecerli bir karakter (enter veya esc) girilene kadar karakter almaya devam et

                        break;
                    }
                }
            }

        }

        closesocket(soket);
    }
    catch (const char* hata_mesaji)
    {
        cout << hata_mesaji << endl;        //socket'te veya kutuphanede hata cikarsa gonderilen hata mesajini yazdir
    }

    return 0;
}