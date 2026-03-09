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

typedef std::chrono::high_resolution_clock Clock;

class TcpIpServer
{
	public:
        TcpIpServer();
        TcpIpServer(long comport);
        ~TcpIpServer();

        void TcpIpInit();
        void TcpIpLoop();
        void CheckConnections();
        void ProcessRequests();

        long port;
	int client_socket[30];
	int max_clients;
	char buffer[1025];  // 1k data buffer
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


	
	private:
};



/*

	bool roofConnectedReport=false;
	bool weatherConnectedReport=false;
	unsigned long roofTimeOffset = 0;
	unsigned long weatherTimeOffset = 0;
	int roofPosition;
	bool isServer;
	bool connected;

//*/




#endif
