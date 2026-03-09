#include "AppSettings.h"

AppSettings::AppSettings()
{
    delimiter = "\n";
    commentOutChar = "#";
    assignmentChar = "=";
}

AppSettings::AppSettings(std::string fileName)
{
    settingsFileName = fileName;
    delimiter = "\n";
    commentOutChar = "#";
    assignmentChar = "=";
}

AppSettings::~AppSettings()
{
}

void AppSettings::SetSettingsFilename(std::string fileName)
{
    settingsFileName = fileName;
}

void AppSettings::SetDelimiter(std::string newDelimiter)
{
    delimiter = newDelimiter;
}


void AppSettings::SaveSettings()
{
    std::ofstream outStream(settingsFileName);
    outStream << dataBuffer;
    outStream.close();
}

int AppSettings::ReadSettings()
{
    dataBuffer.clear();
    std::ifstream ifStream(settingsFileName);
    if (ifStream.is_open())
    {
        std::string line;
        while ( getline (ifStream,line) )
        {
          dataBuffer += line;
          dataBuffer += delimiter;
        }
    }
    else
    {
        std::cout << "Could not read settings file\n";
        return 0;
    }
    ifStream.close();
    return 1;
}

void AppSettings::AppendSetting(std::string setting)
{
    dataBuffer += setting;
    dataBuffer += delimiter;
}

std::string AppSettings::GetSetting(std::string settingName)
{
    std::string result, settingString;
    result.clear();
    if( dataBuffer.length() == 0 )
    {
        return result;
    }
    size_t endIndex;
    size_t settingsNameLength = settingName.length();
    size_t startIndex = dataBuffer.find(settingName);
    size_t assignerIndex = 0;
    size_t settingIndex = 0;
    for(size_t i=startIndex; i<dataBuffer.length(); i++)
    {
        if( assignerIndex && dataBuffer.substr(i,1) != " " && !settingIndex )
        {
            settingIndex = i;
        }
        if( dataBuffer.substr(i,1) == assignmentChar )
        {
            assignerIndex = i;
        }
        if( dataBuffer.substr(i,1) == delimiter )
        {
            endIndex = i;
            break;
        }
    }

    size_t settingsLength = endIndex - startIndex;
    settingString = dataBuffer.substr(startIndex, settingsLength);
    size_t settingStringLength = settingString.length();
    for( int i = startIndex; i>=0; i-- ) //detect #
    {
        if(dataBuffer.substr(i,1) == delimiter)
        {
            break;
        }
        if( dataBuffer.substr(i,1) == commentOutChar )
        {
            return result;
        }
    }

    if( assignerIndex )
    {
        result = dataBuffer.substr(settingIndex, (endIndex - settingIndex) );
        while(result.size() && isspace(result.back())) // trim right
        {
            result.pop_back();
            
        }
    }
    else
    {
        std::cout << " = not found\n";
    }
    return result;
}









