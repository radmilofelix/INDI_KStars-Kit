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

#define DEBUG

#define COMPORT 20002

//#define ROOFDRIVERTIMEOUT 30000 // ms
//#define WEATHERDRIVERTIMEOUT 30000 // ms
#define CLIENTTIMEOUT 30000 // ms

struct CLIENTDATA
{
    char name[50];
//    int clientConnect;
    int socketID;
    char hartbeatCallFromClient[50];
    char hartbeatResponseToClient[50];
    time_t clientTimer;
};

class TcpIpServer
{
    public:
        TcpIpServer(int clients);
        TcpIpServer(int clients, long comport);
        ~TcpIpServer();

        void TcpIpLoop();
        void SetHartbeatMessages(int index, char *callFromClient, char *responseToClient);
//        void ProcessRequests();

    private:
        void DataInit();
        void TcpIpInit();
        void CheckConnections();
        void Hartbeat();

        long port;
        int *client_socket;
        char buffer[1025];  // 1k data buffer
        int master_socket;
        int new_socket;
        struct sockaddr_in address;
        int addrlen;
        int sd; // socket descriptor
        struct timeval tv;
        int maxNumberOfClients;
        CLIENTDATA *clientData;
};


#endif
