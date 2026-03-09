#include "TcpIpClient.h"


TcpIpClient::TcpIpClient()
{
    strcpy(hostAddress, "127.0.0.1");
    port = 20000;
    Setup();
    TcpIpInit();
}

TcpIpClient::TcpIpClient(long comport)
{
    strcpy(hostAddress, "127.0.0.1");
    port = comport;
    Setup();
    TcpIpInit();
}

TcpIpClient::TcpIpClient(char *address, long comport)
{
    port = comport;
    strcpy(hostAddress, address);
    Setup();
    TcpIpInit();
}

TcpIpClient::~TcpIpClient()
{
    close(sockfd);
}

void TcpIpClient::Setup()
{
    clientTime = time(0);
    SetHartbeat((char*)"HartbeatClient-1", (char*)"HartbeatServer-1");
    clientTimeout = 5;
}

void TcpIpClient::SetHartbeat(char *clientToServer, char *serverToClient)
{
    strcpy( hartbeatFromServer, serverToClient );
    strcpy( hartbeatToServer, clientToServer );
}

void TcpIpClient::SetClientTimeout(int newTimeout)
{
    clientTimeout = newTimeout;
}

void TcpIpClient::TcpIpInit()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        printf("ERROR opening socket\n");
    server = gethostbyname(hostAddress);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("ERROR connecting\n");
        exit(0);
    }
}


void TcpIpClient::TcpIpLoop()
{
	Hartbeat();
//    ProcessRequests();
}


//void TcpIpClient::ProcessRequests()
//{
//}


void TcpIpClient::Hartbeat()
{
    time_t currentTime = time(0);

    if( (currentTime - clientTime) > clientTimeout )
    {
        strcpy(buffer, hartbeatToServer);
        int n = write(sockfd, buffer, strlen(buffer));
#ifdef DEBUG
        printf("Sent: %s\n", buffer);
#endif
        buffer[0] = 0;
        n = read(sockfd, buffer, 1024);
        buffer[strcspn(buffer, "\r")] = 0;
        buffer[strcspn(buffer, "\n")] = 0;

        if (n < 0)
            printf("ERROR reading from socket\n");

        if(strcmp(buffer, hartbeatFromServer))
        {
            printf("Wrong hartbeat received\n");
        }
#ifdef DEBUG
        printf("Received: %s\n", buffer);
#endif
        clientTime = currentTime;
    }
}

