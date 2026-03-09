all: TcpIpClientTest TcpIpClientTest1

TcpIpClient.o: TcpIpClient.cpp
	g++ -c TcpIpClient.cpp

TcpIpClientTest.o: TcpIpClientTest.cpp
	g++ -c TcpIpClientTest.cpp

TcpIpClientTest: TcpIpClientTest.o TcpIpClient.o
	g++ TcpIpClientTest.o TcpIpClient.o -o TcpIpClientTest

TcpIpClientTest1.o: TcpIpClientTest1.cpp
	g++ -c TcpIpClientTest1.cpp

TcpIpClientTest1: TcpIpClientTest1.o TcpIpClient.o
	g++ TcpIpClientTest1.o TcpIpClient.o -o TcpIpClientTest1

clean:
	rm -rf *o TcpIpClientTest TcpIpClientTest1
	
