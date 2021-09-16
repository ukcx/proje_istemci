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
    ClientOperations();
    ~ClientOperations();
    bool initializeSocket(std::string address, int port);
    bool connectToServer();
    bool sendMessage(std::string msg);
    bool receiveMessage(std::string & msg);   //read, recv ile soket uzerinden mesaji okuyoruz

private:
    SOCKET createSocket();

    SOCKET client_socket;
    sockaddr_in server_info;            //soket icin gerekli adres bilgileri
    std::string server_address;
    int port;
};

extern ClientOperations* client;

ClientOperations::ClientOperations()
{}

ClientOperations::~ClientOperations()
{
    closesocket(client_socket);
}

bool ClientOperations::initializeSocket(std::string address, int p) 
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return false;

    server_address = address;
    port = p;

    client_socket = createSocket();
    
    if (client_socket == INVALID_SOCKET)
        return false;

    return true;
}

SOCKET ClientOperations::createSocket()
{
    memset(&server_info, 0, sizeof(server_info));
    server_info.sin_family = AF_INET;                                   //IPv4
    server_info.sin_addr.s_addr = inet_addr(server_address.c_str());   //sunucu IP adresi
    server_info.sin_port = htons(port);

    return socket(AF_INET, SOCK_STREAM, 0);     //SOCK_STREAM TCP/IP protokolune uygun olan sabit
}

bool ClientOperations::connectToServer()        //sunucuya baglan
{
    if (connect(client_socket, (sockaddr*)&server_info, sizeof(server_info)) < 0){
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

bool ClientOperations::receiveMessage(std::string &msg)
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
