#include "TcpIpServer.h"

TcpIpServer::TcpIpServer()
{
    port = 20000;
}

TcpIpServer::TcpIpServer(long comport)
{
    port = comport;
}

TcpIpServer::~TcpIpServer()
{
}

void TcpIpServer::TcpIpInit()
{
    //	isServer=true;
    //	connected=false;
    //	opt=1;
    strcpy(message, "ECHO Daemon v1.0"); // a message
    max_clients = 30;
    //initialise all client_socket[] to 0 so not checked
    for (int i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }
    //create a master socket
    if( (master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        return;
    }
    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        return;
    }
    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        return;
    }
    fprintf(stderr, "Listener on port %ld \n", port);
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        return;
    }
    addrlen = sizeof(address);
}

void TcpIpServer::CheckConnections()
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

void TcpIpServer::TcpIpLoop()
{
    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(master_socket, &readfds);
    max_sd = master_socket;

    //add child sockets to set
    for (int i = 0 ; i < max_clients ; i++)
    {
        //socket descriptor
        sd = client_socket[i];

        //if valid socket descriptor then add to read list
        if(sd > 0)
            FD_SET( sd, &readfds);

        //highest file descriptor number, need it for the select function
        if(sd > max_sd)
            max_sd = sd;
    }

    //wait for an activity on one of the sockets , timeout is NULL ,
    //so wait indefinitely - !!!!! changed to 10 sec. timeout
    tv.tv_sec = 10;             /* 10 second timeout */
    tv.tv_usec = 0;
    activity = select( max_sd + 1, &readfds, NULL, NULL, &tv);
    //        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    if ((activity < 0) && (errno != EINTR))
    {
        printf("select error");
    }

    //If something happened on the master socket ,
    //then its an incoming connection
    if (FD_ISSET(master_socket, &readfds))
    {
        if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        //inform user of socket number - used in send and receive commands
        fprintf(stderr, "New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket, inet_ntoa(address.sin_addr),
                ntohs(address.sin_port));
        roofDriverTime = Clock::now();
        weatherDriverTime = Clock::now();
        roofTimeOffset = 0;
        weatherTimeOffset = 0;
        //add new socket to array of sockets
        for (int i = 0; i < max_clients; i++)
        {
            //if position is empty
            if( client_socket[i] == 0 )
            {
                client_socket[i] = new_socket;
                fprintf(stderr, "Adding to list of sockets as %d\n", i);

                break;
            }
        }
    }
    //else its some IO operation on some other socket
    for (int i = 0; i < max_clients; i++)
    {
        sd = client_socket[i];

        if (FD_ISSET( sd, &readfds))
        {
            //Check if it was for closing , and also read the
            //incoming message
            if ((valread = read( sd, buffer, 1024)) == 0)
            {
                //Somebody disconnected , get his details and print
                getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                fprintf(stderr, "Host disconnected , ip %s , port %d \n",
                        inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                //Close the socket and mark as 0 in list for reuse
                if(roofDriverConnect == sd)
                {
                    roofDriverConnect = 0;
//                    ReportRoofDriverConnect();
                }
                if(weatherDriverConnect == sd)
                {
                    weatherDriverConnect = 0;
//                    ReportWeatherDriverConnect();
                }
                client_socket[i] = 0;
                close(sd);
            }
            else
            {
                //set the string terminating NULL byte on the end
                //of the data read
                buffer[valread] = '\0';
                ProcessRequests();
            }
        }
    }
    CheckConnections();
    usleep(10000);
}

void TcpIpServer::ProcessRequests()
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
