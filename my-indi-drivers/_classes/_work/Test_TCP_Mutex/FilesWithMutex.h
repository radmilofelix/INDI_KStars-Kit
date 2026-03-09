#ifndef __FILESWITHMUTEX_H
#define __FILESWITHMUTEX_H

#include <cstdio>
#include <chrono>
#include <unistd.h>
#include <sys/stat.h>

#define MUTEXOPERATIONWAIT 10000 // us


class FilesWithMutex
{
    public:
    FilesWithMutex(char *mFileName, char *mContolFileName, char* mWriteBuffer);
    ~FilesWithMutex();
    int Read(int bytesToRead);
    int Write(int bytesToWrite);
    
    FILE *dataFile, *controlFile;
    char *dataBuffer;
    char *dataFileName;
    char *controlFileName;
    time_t operationTimer;
    int operationTimeout;
    int operationRetries;
};



#endif
