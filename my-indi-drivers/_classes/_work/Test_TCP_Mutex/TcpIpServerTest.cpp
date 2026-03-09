#include "TcpIpServer.h"

TcpIpServer tcpipserver(3, 22222);


int main()
{
    tcpipserver.SetHartbeatMessages(0, (char*)"yoohoo", (char*)"2u2");
    while (1)
    {
        tcpipserver.TcpIpLoop();
        usleep(100000);
    }
}
