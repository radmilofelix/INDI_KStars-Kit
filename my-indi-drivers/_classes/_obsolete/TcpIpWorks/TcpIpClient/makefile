all: TcpIpClientTest

TcpIpClient.o: TcpIpClient.cpp
	g++ -c TcpIpClient.cpp

TcpIpClientTest.o: TcpIpClientTest.cpp
	g++ -c TcpIpClientTest.cpp

TcpIpClientTest: TcpIpClientTest.o TcpIpClient.o
	g++ TcpIpClientTest.o TcpIpClient.o -o TcpIpClientTest


clean:
	rm -rf *o TcpIpClientTest
	
