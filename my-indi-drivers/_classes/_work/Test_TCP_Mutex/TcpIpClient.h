#ifndef TCPIPIPSERVER__H
#define TCPIPSERVER__H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <chrono>


#define DEBUG

#define COMPORT 20002

struct CLIENTDATA
{
    char name[50];
    int socketID;
    char hartbeatCallFromClient[50];
    char hartbeatResponseToClient[50];
    time_t clientTimer;
};

class TcpIpClient
{
    public:
        TcpIpClient();
        TcpIpClient(long comport);
        TcpIpClient(char *address, long comport);
        ~TcpIpClient();

        void SetHartbeat(char *serverToClient, char *clientToServer);
        void SetClientTimeout(int newTimeout);
        void TcpIpLoop();
//        void ProcessRequests();

    private:
        void Setup();
        void TcpIpInit();
        void Hartbeat();

        char hartbeatToServer[50];
        char hartbeatFromServer[50];
        int sockfd;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        time_t clientTime;
        long port;
        char hostAddress[50];
        char buffer[1025];  // 1k data buffer
        int clientTimeout;
};


#endif
