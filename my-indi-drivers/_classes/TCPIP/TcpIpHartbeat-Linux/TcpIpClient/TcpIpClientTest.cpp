#include "TcpIpClient.h"

TcpIpClient tcpipclient((char*)"127.0.0.1", 22222);

void setup()
{
    tcpipclient.SetHartbeat((char*)"RorMagIp-Connect", (char*)"RorMagIp-Connect-Ok");
    tcpipclient.TcpIpInit();
}

int main()
{
    setup();
    while (1)
    {
        tcpipclient.TcpIpLoop();
    }
}
