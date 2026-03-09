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
#include <string>

#include <iostream>
//#include <sys/types.h>
#include <unistd.h>

using namespace std;

#define DEBUG

#define COMPORT 20002

struct CLIENTDATA
{
    char name[50];
    int socketID;
    char hartbeatCallFromClient[50];
    char hartbeatResponseToClient[50];
};

class TcpIpClient
{
    public:
        TcpIpClient();
        TcpIpClient(long comport);
        TcpIpClient(char *address, long comport);
        ~TcpIpClient();

        void Setup();
        void SetHartbeat(char *serverToClient, char *clientToServer);
        void SetClientTimeout(int newTimeout);
        void TcpIpInit();
        void TcpIpLoop();
        void Hartbeat();
 
        
        int SendString(string mystring);
        int Receive(char* buffer, int dimension);


        long port;
        char hostAddress[50];
        char buffer[1025];  // 1k data buffer

        char hartbeatToServer[50];
        char hartbeatFromServer[50];
        int clientTimeout;

        int sockfd;
        int n;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        time_t clientTime;
    private:
};


#endif
