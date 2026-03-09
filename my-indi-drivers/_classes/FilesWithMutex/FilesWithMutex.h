#ifndef __FILESWITHMUTEX_H
#define __FILESWITHMUTEX_H

#include <cstdio>
#include <chrono>
#include <unistd.h>
#include <sys/stat.h>
#include <string>

#define MUTEXOPERATIONWAIT 10000 // us


class FilesWithMutex
{
    public:
        FilesWithMutex(std::string mFileName, std::string mContolFileName, void* mWriteBuffer);
        ~FilesWithMutex();
        int Read(int bytesToRead);
        int Write(int bytesToWrite);
        void SetFileName(std::string fileName);
        void SetControlFileName(std::string fileName);
        void DisplayVariables();

        FILE *dataFile, *controlFile;
        void *dataBuffer;
        std::string dataFileName;
        std::string controlFileName;
        time_t operationTimer;
        int operationTimeout;
        int operationRetries;
};

#endif
