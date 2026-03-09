CC  = g++
CFLAGS  = -I. -Wall

all: Daemonize_SysVinit clean

Daemonize_SysVinit: Daemonize_SysVinit.o
	$(CC) -o $@ $^
	chmod 700 Daemonize_SysVinit

#Daemonize_SysVinit.o: Daemonize_SysVinit.cpp
#	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
