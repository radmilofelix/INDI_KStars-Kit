#include "TcpIpClient.h"

TcpIpClient tcpipclient((char*)"127.0.0.1", 22222);

void setup()
{
    tcpipclient.SetHartbeat((char*)"HartbeatClient-2", (char*)"HartbeatServer-2");
}

int main()
{
    setup();
    while (1)
    {
        tcpipclient.TcpIpLoop();
        usleep(500000);
    }
}
