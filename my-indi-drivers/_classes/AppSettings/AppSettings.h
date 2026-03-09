#ifndef __APPSETTINGD_H
#define __APPSETTINGD_H

#include <string>
#include <fstream>
#include <iostream>


class AppSettings
{
    public:
    AppSettings();
    AppSettings(std::string fileName);
    ~AppSettings();
    void SetSettingsFilename(std::string fileName);
    void SaveSettings();
    int ReadSettings();
    void AppendSetting(std::string setting);
    std::string GetSetting(std::string settingName);
    void SetDelimiter(std::string newDelimiter);

    std::string settingsFileName;
    std::string dataBuffer;
    std::string delimiter;
    std::string commentOutChar;
    std::string assignmentChar;
    
    private:
};






#endif
