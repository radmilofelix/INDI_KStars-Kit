#include "TcpIpServer.h"


TcpIpServer::TcpIpServer(int clients)
{
    port = 20000;
    maxNumberOfClients = clients;
    clientData = new CLIENTDATA[maxNumberOfClients];
    client_socket = new int[maxNumberOfClients];
    DataInit();
    TcpIpInit();
}

TcpIpServer::TcpIpServer(int clients, long tcpPort)
{
    port = tcpPort;
    maxNumberOfClients = clients;
    clientData = new CLIENTDATA[maxNumberOfClients];
    client_socket = new int[maxNumberOfClients];
    DataInit();
    TcpIpInit();
}

TcpIpServer::~TcpIpServer()
{
}

void TcpIpServer::SetTcpPort(long tcpPort)
{
    int result = shutdown (master_socket, 0);
    if( result != 0 )
    {
        fprintf(stderr, "Error closing socket");
        return;
    }
        for(int i = 0; i < maxNumberOfClients; i++)
    {
        (clientData+i)->clientTimer = time(0);
        *(client_socket+i) = 0;
    }
    fprintf(stderr, "Socket change - closing old  socket\n");
    port = tcpPort;
    TcpIpInit();
}

void TcpIpServer::SetHartbeatMessages(int index, char *callFromClient, char *responseToClient)
{
    strcpy( (clientData + index)->hartbeatCallFromClient, callFromClient);
    strcpy( (clientData + index)->hartbeatResponseToClient, responseToClient);
}


void TcpIpServer::DataInit()
{
    char index[10];
    for(int i = 0; i < maxNumberOfClients; i++)
    {
        (clientData+i)->clientTimer = time(0);
        strcpy(index, (const char*)std::to_string(i).c_str());
        strcpy((clientData+i)->hartbeatCallFromClient, "HartbeatClient-");
        strcat((clientData+i)->hartbeatCallFromClient, index);
        strcpy((clientData+i)->hartbeatResponseToClient, "HartbeatServer-");
        strcat((clientData+i)->hartbeatResponseToClient, index);
        *(client_socket+i) = 0;
    }
}

void TcpIpServer::TcpIpInit()
{
    //create a master socket
    if( (master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        fprintf(stderr, "TCPIP-socket failed\n");
        exit(0);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    int opt;
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        fprintf(stderr, "TCPIP-Can not set master socket to allow multiple connections\n");
        exit(0);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        fprintf(stderr, "TCPIP-bind failed\n");
        exit(0);
    }
    fprintf(stderr, "TCPIP-Listening on port %ld \n", port);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        fprintf(stderr, "TCPIP-listen error\n");
        exit(0);
    }
    addrlen = sizeof(address);
}

void TcpIpServer::CheckConnections()
{
#ifdef DEBUG
    //	printf("Checking connections\n");
#endif
    time_t currentTime = time(0);
    for( int i=0; i < maxNumberOfClients; i++)
    {
        if( (clientData+i)->socketID ) // active client
        {
            if( (currentTime - (clientData+i)->clientTimer) >  CLIENTTIMEOUT ) // check if the client timer is expired
            {
                for ( int j = 0; j < maxNumberOfClients; j++ )
                {
                    if( *(client_socket+j) == (clientData+i)->socketID )
                    {
                        *(client_socket+j) = 0;
                    }
                }
                close( (clientData+i)->socketID );
                    fprintf(stderr, "TCPIP-Lost connection with the client, disconnecting. Timeout: %lu sec, Socket: %d\n", currentTime - (clientData+i)->clientTimer, (clientData+i)->socketID );
                (clientData+i)->socketID = 0;
            }
        }
    }
}


void TcpIpServer::TcpIpLoop()
{
    fd_set readfds;
    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(master_socket, &readfds);
    int max_sd = master_socket;

    //add child sockets to set
    for (int i = 0 ; i < maxNumberOfClients ; i++)
    {
        //socket descriptor
        sd = *(client_socket+i);

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
    int activity;
    activity = select( max_sd + 1, &readfds, NULL, NULL, &tv);
    if ((activity < 0) && (errno != EINTR))
    {
        fprintf(stderr, "TCPIP-select error");
    }

    //If something happened on the master socket ,
    //then its an incoming connection
    if (FD_ISSET(master_socket, &readfds))
    {
        if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            fprintf(stderr, "TCPIP-accept error\n");
            exit(EXIT_FAILURE);
        }
        //inform user of socket number - used in send and receive commands
        fprintf(stderr, "TCPIP-New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket, inet_ntoa(address.sin_addr),
                ntohs(address.sin_port));

        //add new socket to array of sockets
        for (int i = 0; i < maxNumberOfClients; i++)
        {
            //if position is empty
            if( *(client_socket+i) == 0 )
            {
                *(client_socket+i) = new_socket;
                fprintf(stderr, "TCPIP-Adding to list of sockets as %d\n", i);

                break;
            }
        }
    }

    //else its some IO operation on some other socket
    for (int i = 0; i < maxNumberOfClients; i++)
    {
        sd = *(client_socket+i);

        if (FD_ISSET( sd, &readfds))
        {
            //Check if it was for closing , and also read the
            //incoming message
            int valread;
            if ((valread = read( sd, buffer,  BUFFERLENGTH)) == 0)
            {
                //Somebody disconnected , get his details and print
                getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                fprintf(stderr, "TCPIP-Host disconnected , ip %s , port %d \n",
                        inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                //Close the socket and mark as 0 in list for reuse

                for(int j = 0; j < maxNumberOfClients; j++)
                {
                    if( (clientData+j)->socketID == sd )
                    {
                        (clientData+j)->socketID = 0;
                    }
                }

                *(client_socket+i) = 0;
                close(sd);
            }
            else // communication from client
            {
                //set the string terminating NULL byte on the end
                //of the data read
                buffer[valread] = '\0';
                Hartbeat(); // communicating socket in sd 
            }
        }
    }
    CheckConnections();
    usleep(10000);
}


void TcpIpServer::Hartbeat()
{
    buffer[strcspn(buffer, "\r")] = 0;
    buffer[strcspn(buffer, "\n")] = 0;

    for(int i = 0; i < maxNumberOfClients; i ++)
    {

        if(!strcmp(buffer, (clientData+i)->hartbeatCallFromClient)) // hartbeat message from client recognised
        {
            (clientData+i)->socketID = sd; // store the socket descriptor
            (clientData+i)->clientTimer = time(0); // reset client timer
            char rdBuffer[30];
            strcpy(rdBuffer, (clientData+i)->hartbeatResponseToClient); // hartbeat response to client
            rdBuffer[strlen(rdBuffer)] = 0;
            send(sd, rdBuffer, strlen(rdBuffer)+1, 0 );
    #ifdef DEBUG
            printf("Hartbeat Received: %s\n", buffer);
            rdBuffer[strcspn(rdBuffer, "\r")] = 0;
            rdBuffer[strcspn(rdBuffer, "\n")] = 0;
            printf("Hartbeat Sent: %s\n", rdBuffer);
    #endif
        }
    } 
}
