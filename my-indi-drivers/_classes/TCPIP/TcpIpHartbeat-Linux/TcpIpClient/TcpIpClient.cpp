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
    SetHartbeat((char*)"yohoo", (char*)"2u2");
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
        printf("ERROR connecting\n");
}


void TcpIpClient::TcpIpLoop()
{
    Hartbeat();
}


void TcpIpClient::Hartbeat()
{
    time_t currentTime = time(0);
//    char rdBuffer[30];
    if( (currentTime - clientTime) > clientTimeout )
    {
        strcpy(buffer, hartbeatToServer);
        n = write(sockfd, buffer, strlen(buffer));
        buffer[0] = 0;
        usleep(20000);
        n = read(sockfd, buffer, 1024);
        buffer[n] = 0;
        buffer[strcspn(buffer, "\r")] = 0;
        buffer[strcspn(buffer, "\n")] = 0;

        if (n < 0)
        {
            printf("ERROR reading from socket\n");
        }

        if(strcmp(buffer, hartbeatFromServer))
        {
            printf("Wrong hartbeat answer received\n");
        }
#ifdef DEBUG
//        printf("Received chars: %d\n", n);
//        printf("Received: %s\n", buffer);
#endif
        clientTime = currentTime;
    }
}

int TcpIpClient::SendString(string myString)
{
//    n = write(sockfd, myString.c_str(), myString.size() + 1);
//    if( n < 0 )

    if( -1 == send(sockfd, myString.c_str(), myString.size() + 1, 0))
    {
        printf("Could not send data to server!\r\n");
        return 0;
    }
    return 1;
}


int TcpIpClient::Receive(char* locbuffer, int dimension)
{
    memset(locbuffer, 0, dimension);
//    int bytesReceived = recv(sockfd, locbuffer, dimension, 0);
    int bytesReceived = recv(sockfd, locbuffer, dimension, MSG_DONTWAIT);
//    int bytesReceived = recv(sockfd, locbuffer, dimension, MSG_WAITALL);
    if(-1 == bytesReceived)
    {
        printf("Could not receive response from server!\r\n");
        return 0;
    }
    return bytesReceived;
}


