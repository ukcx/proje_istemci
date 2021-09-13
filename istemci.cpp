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
int mesaj_indexi = 1;

string convertToLowerCase(string str)   //string kontrolu icin
{
    string lowerCaseStr = "";
    for (int i = 0; i < str.length(); i++)
    {
        lowerCaseStr += tolower(str[i]);
    }
    return lowerCaseStr;
}

SOCKET socket_olustur(sockaddr_in& sunucu_bilgileri)
{
    memset(&sunucu_bilgileri, 0, sizeof(sunucu_bilgileri));
    sunucu_bilgileri.sin_family = AF_INET;                                 //IPv4 
    sunucu_bilgileri.sin_addr.s_addr = inet_addr(sunucu_adresi.c_str());   //sunucu IP adresi
    sunucu_bilgileri.sin_port = htons(sunucu_portu);

    return socket(AF_INET, SOCK_STREAM, 0);     //SOCK_STREAM TCP/IP protokolune uygun olan sabit
}

int mesaj_kontrol(string to, string cc, string subject, string body, string oncelik_seviyesi, string yazdirilacak_yer,
    string& hata_mesaji)
{
    hata_mesaji = "";
    try
    {
        convertStringToPriorityLevel(oncelik_seviyesi);             //oncelik seviyesi dogru girilmediyse mesaj olusturmuyoruz
    }
    catch (const char* error_message)
    {
        hata_mesaji = error_message;
    }

    auto arayici = mesajin_yazdirilinabilecegi_yerler.find(yazdirilacak_yer);
    if (arayici == mesajin_yazdirilinabilecegi_yerler.end())    //kullanicinin girdigi yazdirma yeri gecerli degil
    {
        hata_mesaji = "Gecerli yazdirilacak yer verilmedi!";
    }

    if (hata_mesaji != "")
    {
        return -1;
    }
    else
        return 0;
}

void mesaj_al(string& to, string& cc, string& subject, string& body, string& oncelik_seviyesi, string& yazdirilacak_yer)
{
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
    getline(cin, oncelik_seviyesi);
    cout << "Please enter where do you want your message to be written (database, file, console): ";
    getline(cin, yazdirilacak_yer);

    yazdirilacak_yer = convertToLowerCase(yazdirilacak_yer);
}

int kullanicidan_karakter_al()
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
        default:
            return ch;          //karakteri kullaniciya dondur
            break;
        }
    }
}

int mesaji_gonder(SOCKET socket, string mesaj_yazdirma_yeri, Message& mesaj)
{
    if ((send(socket, mesaj_yazdirma_yeri.c_str(), mesaj_yazdirma_yeri.length(), 0) < 0) ||
        (send(socket, mesaj.messageToString().c_str(), mesaj.messageToString().length(), 0) < 0))
    {
        return -1;
    }
    else
        return 0;
}

int main()
{
    try
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //winsock kutuphanesini baslatmak icin gerekli
        {
            throw "Winsock baslatilamadi.";     /*throw komutu yurutulurse ait olunan try scope'undan cikilir ve direkt
                                                 olarak try scope'unun disinda scope'a en yakin catch fonksiyonuna gidilir*/
        }

        sockaddr_in addr;                   //soket icin gerekli adres bilgileri
        SOCKET soket = socket_olustur(addr);

        if (soket == INVALID_SOCKET)
        {
            throw "Soket olusturulamadi";
        }

        if (connect(soket, (sockaddr*)&addr, sizeof(addr)) < 0)    //sunucuya baglan
        {
            throw "Sunucuya baglanma hatasi";
        }


        cout << "Sunucuya baglanildi" << endl;  //kodda buraya kadar herhangi bir throw komutu yürütülmediyse sunucuya basariyla baglanildi

        bool escapeCharacterIsPressed = false;      //ESC'ye basilmadi

        while (!escapeCharacterIsPressed)       //ESC karakteri girilene kadar mesaj girilmeye devam ediliyor
        {
            string to, cc, subject, body, oncelik_seviyesi, yazdirilacak_yer, hata_mesaji;

            mesaj_al(to, cc, subject, body, oncelik_seviyesi, yazdirilacak_yer);

            if (mesaj_kontrol(to, cc, subject, body, oncelik_seviyesi, yazdirilacak_yer, hata_mesaji) < 0)
            {
                cerr << endl << hata_mesaji << endl;
                cerr << "Mesajiniz gonderilemedi" << endl;
            }

            else
            {
                Message mesaj(to, cc, subject, body, convertStringToPriorityLevel(oncelik_seviyesi));

                if (yazdirilacak_yer == "console")      //o zaman mesaji gondermeden sadece istemci konsolunda yazdir
                {
                    cout << endl << "Mesajiniz: " << endl;
                    mesaj.printMessage();
                }

                if (mesaji_gonder(soket, yazdirilacak_yer, mesaj) < 0) //write, send ile soket uzerine mesaj yaziyoruz
                {
                    cerr << "\nMesaj gonderilemedi" << endl;
                    return 1;             //eger mesaj gonderemezsek sunucunun haberi olmasi icin istemciyi kapat
                }
                else    //mesaj basariyla gonderildi
                {
                    cout << "\nMesaj gonderildi" << endl;

                    char alinan_mesaj[MSG_SIZE];
                    memset(&alinan_mesaj, 0, MSG_SIZE);
                    recv(soket, alinan_mesaj, MSG_SIZE, 0);  //read, recv ile soket uzerinden mesaji okuyoruz

                    cout << endl << mesaj_indexi << "-) Sunucudan alinan mesaj: " << alinan_mesaj << endl;
                    mesaj_indexi++;
                }
            }

            cout << "\nMesaj gondermeye devam etmek icin Enter'a, mesaj gonderim islemini sonlandirmak icin ESC'ye basiniz!" << endl;

            int karakter;
            do
            {
                karakter = kullanicidan_karakter_al();

            } while ((karakter != 0x1b) && (karakter != 0x0d));   //ENTER veya ESC girilene kadar karakter almaya devam et

            if (karakter == 0x1b)
            {
                cout << "\ESC'ye basildi" << endl;
                escapeCharacterIsPressed = true;
            }
            else
            {
                cout << "\nEnter'a basildi" << endl;
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