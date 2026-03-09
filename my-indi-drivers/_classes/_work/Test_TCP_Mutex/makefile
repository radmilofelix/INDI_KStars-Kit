all: TcpIpServerTest TcpIpClientTest TcpIpClientTest1


FilesWithMutex.o: FilesWithMutex.cpp
	g++ -c FilesWithMutex.cpp

TcpIpServer.o: TcpIpServer.cpp
	g++ -c TcpIpServer.cpp

TcpIpServerTest.o: TcpIpServerTest.cpp
	g++ -c TcpIpServerTest.cpp

TcpIpServerTest: TcpIpServerTest.o TcpIpServer.o FilesWithMutex.o
	g++ TcpIpServerTest.o TcpIpServer.o FilesWithMutex.o -o TcpIpServerTest


TcpIpClient.o: TcpIpClient.cpp
	g++ -c TcpIpClient.cpp

TcpIpClientTest.o: TcpIpClientTest.cpp
	g++ -c TcpIpClientTest.cpp

TcpIpClientTest: TcpIpClientTest.o TcpIpClient.o FilesWithMutex.o
	g++ TcpIpClientTest.o TcpIpClient.o FilesWithMutex.o -o TcpIpClientTest

TcpIpClientTest1.o: TcpIpClientTest1.cpp
	g++ -c TcpIpClientTest1.cpp

TcpIpClientTest1: TcpIpClientTest1.o TcpIpClient.o FilesWithMutex.o
	g++ TcpIpClientTest1.o TcpIpClient.o FilesWithMutex.o -o TcpIpClientTest1


clean:
	rm -rf *o TcpIpServerTest TcpIpClientTest TcpIpClientTest1
	
