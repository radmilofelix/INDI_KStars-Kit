#include "FilesWithMutex.h"


FilesWithMutex::FilesWithMutex(char *mFileName, char *mContolFileName, char* mWriteBuffer)
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
     delete dataBuffer;
}

int FilesWithMutex::Read(int bytesToRead)
{
    struct stat st;
    while(stat(controlFileName,&st) == 0 && operationRetries)
    {
        operationRetries--;
//      printf("controlFileName present at read time - %s\n", controlFileName);
        usleep(MUTEXOPERATIONWAIT);
//      return -1;
    }
    if(operationRetries==0)
    {
        printf("controlFileName present at read time, timeout reached - %s\n", controlFileName);
        operationRetries=4;
        return 0;
    }
    operationRetries=4;

    controlFile=fopen(controlFileName,"w");
    if(controlFileName == NULL)
    {
        fprintf(stderr, "Error opening lock file - %s\n",controlFileName);
        return 0;
    }
    fclose(controlFile);

    dataFile=fopen(dataFileName,"r");
    if (dataFile == NULL)
    {
        fprintf(stderr, "Error opening file to read - %s\n", dataFileName);
        return 0;
    }
    fread(dataBuffer,bytesToRead, 1, dataFile);
    fclose(dataFile);
    std::remove(controlFileName);
    return 1;
}

int FilesWithMutex::Write(int bytesToWrite)
{
    struct stat st;
    while(stat(controlFileName,&st) == 0 && operationRetries)
    {
        operationRetries--;
//      printf("controlFileName present at write time - %s\n", controlFileName);
        usleep(MUTEXOPERATIONWAIT);
//      return -1;
    }
    if(operationRetries==0)
    {
        printf("controlFileName present at write time, timeout reached - %s\n", controlFileName);
        operationRetries=4;
        return 0;
    }
    operationRetries=4;

    controlFile=fopen(controlFileName,"w");
    if(controlFile == NULL)
    {
        fprintf(stderr, "Error opening lock file - %s\n",controlFileName);
        return 0;
    }
    fclose(controlFile);
    dataFile=fopen(dataFileName,"w");
    if (dataFile == NULL)
    {
        fprintf(stderr, "Error opening file to write - %s\n",dataFileName);
        return 0;
    }
    fwrite(dataBuffer,bytesToWrite+1, 1, dataFile);
    fclose(dataFile);
    return 1;
}
