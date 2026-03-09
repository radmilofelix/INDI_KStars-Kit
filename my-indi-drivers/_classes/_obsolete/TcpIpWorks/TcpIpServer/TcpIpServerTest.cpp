#include "TcpIpServer.h"

TcpIpServer tcpipserver(22222);
//TcpIpServer tcpipserver;

void setup()
{
	tcpipserver.TcpIpInit();
}

int main()
{
	setup();
	while (1)
	{
		tcpipserver.TcpIpLoop();
	}
}
