all: TcpIpServerTest

TcpIpServer.o: TcpIpServer.cpp
	g++ -c TcpIpServer.cpp

TcpIpServerTest.o: TcpIpServerTest.cpp
	g++ -c TcpIpServerTest.cpp

TcpIpServerTest: TcpIpServerTest.o TcpIpServer.o
	g++ TcpIpServerTest.o TcpIpServer.o -o TcpIpServerTest


clean:
	rm -rf *o TcpIpServerTest
	
