#include "Utility.hpp"

#include <iostream>
#include <algorithm>
#include <functional>

//---------------------------------------------------------------------------
//                             const section
//---------------------------------------------------------------------------
const std::string kCommandLineHelp("Specified arguments was wrong!"
                                   "You must at least specify these "
                                   "options as arguments:\n-p port_number"
                                   "\n-s serial_port_name"
                                   "\n-b serial_port_baudrate.\n"
                                   "\n-d enable/disable interpret runtime"
                                   " option:1=enable, 0=disable."
                                   "For example:tcp2serial -p 2101 -s "
                                   "/dev/sttyS2 -b 115200 -d 0\n"
                                   "Next options are optional.You can change "
                                   "them in runtime:\n-c command type.Valid"
                                   " values for this option are:0, 1 and 2."
                                   "0 means disable stream output of serial"
                                   " port. 1 means enable stream output of"
                                   " serial port and 2 means simple command."
                                   "In runtime to change this option only "
                                   "use 0 and 1 values.\n-o crlf removing."
                                   "Valid values for this option are 0 and 1."
                                   "1 to remove crlf and 0 to add crlf.");

const std::string kAboutString("tcp2serial is a tcp server that listen on a"
                               " tcp port and waiting for a client request."
                               "It gets request from client and forward "
                               "incoming request to serial port.");

//---------------------------------------------------------------------------
//                             Functions
//---------------------------------------------------------------------------
bool Utility::parseCommandLineArgs(int argc, char *argv[],
               std::unordered_map<char, std::string> &optionList)
{
    bool result = true;
    int numOfEssentialOptions = 0;

    bool isCOptionSet = false;
    bool isOOptionSet = false;
    //It is important to specify port number, serial device and boudrate
    //in command line arguments
    if (argc < 7)
    {
        std::cout << kCommandLineHelp << std::endl;
        result = false;
    }
    else
    {
        for (int i = 1; i < argc; ++i)
        {
            std::string item(argv[i]);
            if (0 == item.compare("-p"))
            {
                std::string portNumber(argv[i + 1]);
                optionList['p'] = portNumber;
                i++;
                numOfEssentialOptions++;
            }
            else if(0 == item.compare("-s"))
            {
                std::string serialPortName(argv[i + 1]);
                optionList['s'] = serialPortName;
                i++;
                numOfEssentialOptions++;
            }
            else if(0 == item.compare("-b"))
            {
                std::string baudRate(argv[i + 1]);
                optionList['b'] = baudRate;
                i++;
                numOfEssentialOptions++;
            }
            else if(0 == item.compare("-d"))
            {
                std::string runtimeInterpret(argv[i + 1]);
                optionList['d'] = runtimeInterpret;
                i++;
                numOfEssentialOptions++;
            }
            else if (0 == item.compare("-c"))
            {
                std::string commandType(argv[i + 1]);
                optionList['c'] = commandType;
                i++;
                isCOptionSet = true;
            }
            else if (0 == item.compare("-o"))
            {
                std::string crlf(argv[i + 1]);
                optionList['o'] = crlf;
                i++;
                isOOptionSet = true;
            }
            else if (0 == item.compare("-h"))
            {
                Utility::printAboutText();
                i++;
            }
        }
        //Check are all essential options specified?
        if (4 == numOfEssentialOptions)
        {
            result = true;
        }
        else
        {
            std::cout << kCommandLineHelp << std::endl;
            result = false;
        }
        if (false == isCOptionSet)
        {
            optionList['c'] = "3";
        }
        if (false == isOOptionSet)
        {
            optionList['o'] = "0";
        }
    }
    return result;
}
//---------------------------------------------------------------------------
void Utility::eraseAllSubStrings(
        std::string &mainString,
        const std::string &stringToErase)
{
    std::string::size_type pos = std::string::npos;
    // Search for the substring in string in a loop untill nothing is found
    while ((pos = mainString.find(stringToErase) )!= std::string::npos)
    {
        // If found then erase it from string
        mainString.erase(pos, stringToErase.length());
    }
}
//---------------------------------------------------------------------------
void Utility::printAboutText()
{
    std::cout << kAboutString << std::endl;
}
//---------------------------------------------------------------------------
