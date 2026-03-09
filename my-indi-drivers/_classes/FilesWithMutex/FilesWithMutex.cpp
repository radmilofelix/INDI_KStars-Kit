#include "FilesWithMutex.h"


FilesWithMutex::FilesWithMutex(std::string mFileName, std::string mContolFileName,  void* mWriteBuffer)
{
    dataFileName = mFileName;
    controlFileName = mContolFileName;
    dataBuffer = mWriteBuffer;
    operationTimer = time(0);
    operationTimeout = 5; // sec.
    operationRetries = 6;
}

FilesWithMutex::~FilesWithMutex()
{
//    delete dataBuffer;
}


void FilesWithMutex::DisplayVariables()
{
    fprintf(stderr, "Control filename - %s\n", controlFileName.c_str());
    fprintf(stderr, "Data filename - %s\n", dataFileName.c_str());
}

int FilesWithMutex::Read(int bytesToRead)
{
    struct stat st;
    while(stat(controlFileName.c_str(), &st) == 0 && operationRetries)
    {
        operationRetries--;
        usleep(MUTEXOPERATIONWAIT);
    }

    if(operationRetries == 0)
    {
        fprintf(stderr, "controlFileName present at read time, timeout reached - %s\n", controlFileName.c_str());
        operationRetries = 4;
        return 0;
    }
    operationRetries = 4;

    controlFile = fopen(controlFileName.c_str(), "w");
    if(controlFile == NULL)
    {
        fprintf(stderr, "Error opening lock file - %s\n", controlFileName.c_str());
        return 0;
    }
    fclose(controlFile);

    dataFile = fopen(dataFileName.c_str(), "r");
    if (dataFile == NULL)
    {
        fprintf(stderr, "Error opening file to read - %s\n", dataFileName.c_str());
        std::remove(controlFileName.c_str());
        return 0;
    }

    fread(dataBuffer, bytesToRead, 1, dataFile);
    fclose(dataFile);
    std::remove(controlFileName.c_str());
    return 1;
}


int FilesWithMutex::Write(int bytesToWrite)
{
    struct stat st;
    while(stat(controlFileName.c_str(), &st) == 0 && operationRetries)
    {
        operationRetries--;
        usleep(MUTEXOPERATIONWAIT);
    }

    if(operationRetries == 0)
    {
        fprintf(stderr, "controlFileName present at write time, timeout reached - %s\n", controlFileName.c_str());
        operationRetries = 4;
        return 0;
    }
    operationRetries = 4;

    controlFile = fopen(controlFileName.c_str(), "w");
    if(controlFile == NULL)
    {
        fprintf(stderr, "Error opening lock file - %s\n", controlFileName.c_str());
        return 0;
    }
    fclose(controlFile);

    dataFile = fopen(dataFileName.c_str(), "w");
    if (dataFile == NULL)
    {
        fprintf(stderr, "Error opening file to write - %s\n", dataFileName.c_str());
        std::remove(controlFileName.c_str());
        return 0;
    }

    fwrite(dataBuffer, bytesToWrite + 1, 1, dataFile);
    fclose(dataFile);
    std::remove(controlFileName.c_str());
    return 1;
}

void FilesWithMutex::SetFileName(std::string fileName)
{
    dataFileName = fileName;
}

void FilesWithMutex::SetControlFileName(std::string fileName)
{
    controlFileName = fileName;
}
