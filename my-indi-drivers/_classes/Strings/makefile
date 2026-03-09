all: parseTest

allclean: cleanobj parseTest

parseTest: parseTest.o StringParse.o
	g++ parseTest.o StringParse.o -o parseTest


parseTest.o: parseTest.cpp
	g++ -c parseTest.cpp


StringParse.o: StringParse.cpp
	g++ -c StringParse.cpp

clean:
	del *.o parseTest.exe

cleanobj:
	del *.o
