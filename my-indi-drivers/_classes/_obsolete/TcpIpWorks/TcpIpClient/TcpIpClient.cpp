#include "TcpIpClient.h"

TcpIpClient::TcpIpClient()
{
    strcpy(hostAddress, "127.0.0.1");
    port = 20000;
}

TcpIpClient::TcpIpClient(long comport)
{
    strcpy(hostAddress, "127.0.0.1");
    port = comport;
}

TcpIpClient::TcpIpClient(char *address, long comport)
{
    port = comport;
    strcpy(hostAddress, address);
}

TcpIpClient::~TcpIpClient()
{
    close(sockfd);
}

void TcpIpClient::TcpIpInit()
{
//    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");
    server = gethostbyname(hostAddress);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        perror("ERROR connecting");
}

/*
void TcpIpClient::CheckConnections()
{
#ifdef DEBUG
    //	printf("Checking connections\n");
#endif
    auto currentTime = Clock::now();
    if(roofDriverConnect)
    {
        if( roofTimeOffset > ROOFDRIVERTIMEOUT )
        {
            for (int i = 0; i < max_clients; i++)
                if(client_socket[i] == roofDriverConnect)
                    client_socket[i] = 0;
            close(roofDriverConnect);
            roofDriverConnect = 0;
            fprintf(stderr, "Lost connection with the roof INDI driver, disconnecting. Timeout: %lu ms\n", roofTimeOffset);
        }
        roofTimeOffset = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - roofDriverTime).count();
    }

    if(weatherDriverConnect)
    {
        if( weatherTimeOffset > WEATHERDRIVERTIMEOUT )
        {
            for (int i = 0; i < max_clients; i++)
                if(client_socket[i] == weatherDriverConnect)
                    client_socket[i] = 0;
            close(weatherDriverConnect);
            weatherDriverConnect = 0;
            fprintf(stderr, "Lost connection with the weather INDI driver, disconnecting. Timeout: %lu ms\n", weatherTimeOffset);
        }
        weatherTimeOffset = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - weatherDriverTime).count();
    }
//    ReportRoofDriverConnect();
//    ReportWeatherDriverConnect();
}
//*/

void TcpIpClient::TcpIpLoop()
{
    n = read(sockfd, buffer, 255);
    if (n < 0) 
         perror("ERROR reading from socket");
//    printf("Bytes read: %d\n",n);
    int k=strlen(buffer);
//    printf("Buffer length: %d\n",k);
    
    printf("%s\n", buffer);
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) 
         perror("ERROR writing to socket");
    bzero(buffer,256);
}



/*
void TcpIpClient::ProcessRequests()
{
#ifdef DEBUG
	printf("Received: %s\n",buffer);
#endif

	  buffer[strcspn(buffer, "\r")] = 0;
	  buffer[strcspn(buffer, "\n")] = 0;

//RorMagIp-Connect\n

	if(!strcmp(buffer,"RorMagIp-Connect"))
	{
		roofDriverConnect=sd;
		roofDriverTime = Clock::now();
		char rdBuffer[30];
		strcpy(rdBuffer, "RorMagIp-Connect-Ok");
		send(sd, rdBuffer, strlen(rdBuffer), 0 );
#ifdef DEBUG
	printf("Sent: %s\n",buffer);
#endif
	}

	if(!strcmp(buffer,"RorWeatherIp-Connect"))
	{
		weatherDriverConnect=sd;
		weatherDriverTime = Clock::now();
		char rdBuffer[30];
		strcpy(rdBuffer, "RorWeatherIp-Connect-Ok");
		send(sd, rdBuffer, strlen(rdBuffer), 0 );
	}
}
//*/
