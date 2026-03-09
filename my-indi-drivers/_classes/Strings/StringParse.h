#ifndef __STRINGPARSE_H
#define __STRINGPARSE_H

#include <iostream>
#include <sstream>
#include <vector>

class StringParse
//class StringParse : public std::string
{
    public:
    StringParse(std::string input);
    StringParse(std::string input, std::string delimiter);
    ~StringParse();
    
    std::vector<std::string> split();
    std::string GetElement(int index);
    
    private:
    std::string inputString, delim;
    std::vector<std::string> vector;
    int counter;
};


#endif
