#include "TcpIpClient.h"

TcpIpClient tcpipclient((char*)"127.0.0.1", 22222);

void setup()
{
    tcpipclient.SetHartbeat((char*)"RorWeatherIp-Connect", (char*)"RorWeatherIp-Connect-Ok");
    tcpipclient.SetClientTimeout(7);
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
