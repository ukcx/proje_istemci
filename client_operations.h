#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS         //inet_addr kullanmak icin
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif


#ifndef CLIENT_OPERATIONS_H
#define CLIENT_OPERATIONS_H

const int MSG_SIZE = 200;

#pragma comment(lib, "Ws2_32.lib")              //winsock2'yi linkleyebilmek icin
#include <string>
#include <winsock2.h>
#include <iostream>
#include <unordered_set>


class ClientOperations
{
public:
    ClientOperations(std::string server_address, const int port);
    ~ClientOperations();
    bool connectToServer();
    bool sendMessage(std::string msg);
    bool recieveMessage(std::string & msg);   //read, recv ile soket uzerinden mesaji okuyoruz

private:
    SOCKET createSocket();

    WSADATA wsa;
    SOCKET client_socket;
    sockaddr_in server_addr;            //soket icin gerekli adres bilgileri
    std::string server_address;
    int port;
};

extern ClientOperations* client;

ClientOperations::ClientOperations(std::string address, const int p)
{
    server_address = address;
    port = p;
    try {

        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //winsock kutuphanesini baslatmak icin gerekli
            throw "Winsock baslatilamadi.";     /*throw komutu yurutulurse ait olunan try scope'undan cikilir ve direkt
                                                  olarak try scope'unun disinda scope'a en yakin catch fonksiyonuna gidilir*/

        client_socket = createSocket();
        if (client_socket == INVALID_SOCKET)
            throw "Soket olusturulamadi";

    }  catch (...) {
        std::cout << "hata";
    }

}

ClientOperations::~ClientOperations()
{
    closesocket(client_socket);
}

SOCKET ClientOperations::createSocket()
{
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;                                 //IPv4
    server_addr.sin_addr.s_addr = inet_addr(server_address.c_str());   //sunucu IP adresi
    server_addr.sin_port = htons(port);

    return socket(AF_INET, SOCK_STREAM, 0);     //SOCK_STREAM TCP/IP protokolune uygun olan sabit
}

bool ClientOperations::connectToServer()        //sunucuya baglan
{
    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        return false;
    }
    return true;
}

bool ClientOperations::sendMessage(std::string msg)
{
    if (send(client_socket, msg.c_str(), msg.length(), 0) < 0)
    {
        return false;
    }
    else
        return true;
}

bool ClientOperations::recieveMessage(std::string &msg)
{
    char received_message[MSG_SIZE];
    memset(&received_message, 0, MSG_SIZE);
    if (recv(client_socket, received_message, MSG_SIZE, 0) < 0)
    {
        return false;
    }
    else
    {
        msg = received_message;
        return true;
    }
}

#endif      // CLIENT_OPERATIONS_H
