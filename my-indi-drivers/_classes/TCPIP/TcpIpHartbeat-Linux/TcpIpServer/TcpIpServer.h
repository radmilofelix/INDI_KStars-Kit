#ifndef TCPIPIPSERVER__H
#define TCPIPSERVER__H

#include <cstdio>
#include <stdlib.h>
#include <string.h>   //strlen  
#include <errno.h>
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/socket.h>
#include <sys/stat.h>
#include <chrono>
#include <string>

//#define DEBUG

#define CLIENTTIMEOUT 20 // sec
#define BUFFERLENGTH 4096

struct CLIENTDATA
{
    int socketID;
    char hartbeatCallFromClient[50];
    char hartbeatResponseToClient[50];
    time_t clientTimer;
};

class TcpIpServer
{
    public:
        TcpIpServer(int clients);
        TcpIpServer(int clients, long tcpPort);
        ~TcpIpServer();

        void SetTcpPort(long tcpPort);
        void TcpIpLoop();
        void SetHartbeatMessages(int index, char *callFromClient, char *responseToClient);

        char buffer[ BUFFERLENGTH];  // data buffer
        CLIENTDATA *clientData;

    private:
        void DataInit();
        void TcpIpInit();
        void CheckConnections();
        void Hartbeat();

        long port;
        int *client_socket;
        int master_socket;
        int new_socket;
        struct sockaddr_in address;
        int addrlen;
        int sd; // socket descriptor
        struct timeval tv;
        int maxNumberOfClients;
};


#endif
