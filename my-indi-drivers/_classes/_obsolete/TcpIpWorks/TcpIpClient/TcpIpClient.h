#ifndef TCPIPIPSERVER__H
#define TCPIPSERVER__H

/*
#include <cstdio>
#include <stdlib.h>
#include <string.h>   //strlen  
#include <errno.h>
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/socket.h>
#include <sys/stat.h>
#include <chrono>
//*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define DEBUG

#define COMPORT 20002

#define ROOFDRIVERTIMEOUT 30000 // ms
#define WEATHERDRIVERTIMEOUT 30000 // ms


struct CLIENTDATA
{
	char name[50];
	int socketID;
	char hartbeatCallFromClient[50];
	char hartbeatResponseToClient[50];
};

//typedef std::chrono::high_resolution_clock Clock;

class TcpIpClient
{
	public:
        TcpIpClient();
        TcpIpClient(long comport);
        TcpIpClient(char *address, long comport);
        ~TcpIpClient();

        void TcpIpInit();
        void TcpIpLoop();
//        void CheckConnections();
//        void ProcessRequests();

        long port;
        char hostAddress[50];
	char buffer[1025];  // 1k data buffer


    int sockfd;
    int n;
    struct sockaddr_in serv_addr;
    struct hostent *server;



/*
	int client_socket[30];
	int max_clients;
	fd_set readfds; // set of socket descriptors
	char message[500]; // a message
	int master_socket;
	int new_socket;

	int opt;
	struct sockaddr_in address;
	int addrlen;
	int sd;
	int max_sd;
	int activity;
	int valread;

	struct timeval tv;

	int roofDriverConnect = 0; // socket descriptors for the 2 drivers
	int weatherDriverConnect = 0; // socket descriptors for the 2 drivers
	unsigned long roofTimeOffset = 0;
	unsigned long weatherTimeOffset = 0;
	
	std::chrono::system_clock::time_point weatherDriverTime = Clock::now();
	std::chrono::system_clock::time_point roofDriverTime = Clock::now();
//*/
	
	private:
};



#endif
