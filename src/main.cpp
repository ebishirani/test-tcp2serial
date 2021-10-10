#include<iostream>
#include <unistd.h>
#include <cstdlib>
#include <unordered_map>
#include <sstream>

#include "SerialPort.hpp"
#include "Utility.hpp"
#include "SyncQueue.hpp"

using namespace linuxserialport;
using namespace ioi::ai_center::four_ch_dvr;
//---------------------------------------------------------------------------
//                          Const section
//---------------------------------------------------------------------------
const std::string kEraseString("\r\n");
//---------------------------------------------------------------------------
//                          Global variables
//---------------------------------------------------------------------------
//This queue will be used to exchange data between producer(Tcp server) and
//consumer(main thread) threads.
SyncQueue<std::string> commonBuffer;
//This variavle will be used to terminate program.
bool mustTerminateApp = false;

bool mustBeRemoveCrlf = false;

void handleRecievedDataFromTcpPort(SerialPort &serialPort)
{
    //Sometimes it is pissible that a command and it's parameters are placed
    // in tow consecutiuve packets.In this situation we hold current section
    // of command in next strig.
    std::string dataFromPrePacket = "";

    while (false == mustTerminateApp)
    {
        std::string currentPacket("");
        commonBuffer.deQueueWithTimeOut(1500, currentPacket);
        currentPacket.insert(0, dataFromPrePacket);
        if (currentPacket.length() < 2)
        {
            dataFromPrePacket = currentPacket;
            continue;
        }
        while (true)
        {
            if (0 == currentPacket.length())
            {
                break;
            }
            if ('-' == currentPacket[0])
            {
                std::string command = currentPacket.substr(0, 2);
                if (0 == command.compare("-h"))
                {
                    Utility::printAboutText();
                    //We erase 3 characters to delet space one.
                    currentPacket.erase(0, 3);
                }
                //-----------------------------------------------------------
                else if (0 == command.compare("-o"))
                {
                    currentPacket.erase(0, 2);
                    if(currentPacket.length() > 1)
                    {
                        //currentPacket[0] must be space
                        if ('0' == currentPacket[1])//remove crlf
                        {
                            mustBeRemoveCrlf = true;
                        }
                        else if ('1' == currentPacket[1])//add crlf
                        {
                            mustBeRemoveCrlf = false;
                        }
                        //We erase 2 characters to delet space one.
                        currentPacket.erase(0, 2);
                        continue;
                    }
                    else
                    {
                        dataFromPrePacket = command + currentPacket;
                        break;
                    }
                }
                //-----------------------------------------------------------
                else if (0 == command.compare("-c"))
                {
                    currentPacket.erase(0, 2);
                    if(currentPacket.length() > 1)
                    {
                        std::string::size_type spacePosition =
                                currentPacket.find(' ', 1);

                        spacePosition = (spacePosition == std::string::npos) ?
                                    spacePosition : spacePosition - 1;

                        std::string commandType =
                                currentPacket.substr(1, spacePosition);

                        currentPacket.erase(0, spacePosition);

                        if (0 == commandType.compare("0"))
                        {
                            serialPort.enableDisableStreamOutput(false);
                        }
                        else if (0 == commandType.compare("1"))
                        {
                            serialPort.enableDisableStreamOutput(true);
                        }
                        else if(commandType.length() > 1)
                        {
                            if (true == mustBeRemoveCrlf)
                            {
                                Utility::eraseAllSubStrings(commandType,
                                                            kEraseString);
                            }
                            serialPort.write(commandType);
                        }
                        else
                        {
                            dataFromPrePacket = command + " ";
                            break;
                        }
                        continue;
                    }
                    else
                    {
                        dataFromPrePacket = command + currentPacket;
                        break;
                    }
                }
                //-----------------------------------------------------------
                else if (0 == command.compare("-q"))
                {
                    mustTerminateApp = true;
                    break;
                }
            }//end of if ('-' == currentPacket[0])
            else
            {
                std::string::size_type dashPosition =
                        currentPacket.find('-', 0);

                if (dashPosition == std::string::npos)
                {
                    if (true == mustBeRemoveCrlf)
                    {
                        Utility::eraseAllSubStrings(currentPacket,
                                                    kEraseString);
                    }
                    serialPort.write(currentPacket);
                    break;
                }
                else
                {
                    dashPosition--;
                    currentPacket.erase(0, dashPosition);
                    std::string dataToSend =
                            currentPacket.substr(0, dashPosition);
                    if (true == mustBeRemoveCrlf)
                    {
                        Utility::eraseAllSubStrings(dataToSend,
                                                    kEraseString);
                    }
                    serialPort.write(dataToSend);
                    continue;
                }
            }

        }//End of while(tru).
    }//End of while (false == mustTerminateApp).
}

int main(int argc, char *argv[])
{
    std::unordered_map<char, std::string> commandLineArgs;

    bool parseResult = Utility::parseCommandLineArgs(argc, argv,
                                                     commandLineArgs);

    if (false == parseResult)
    {
        return 0;
    }
    unsigned int boudrate = std::stoul(commandLineArgs['b'],nullptr,0);
    SerialPort serialPort(commandLineArgs['s'], boudrate);

    if (0 == commandLineArgs['o'].compare("1"))
    {
        mustBeRemoveCrlf = true;
    }
    //SerialPort serialPort("/dev/ttyS1", boudrate);
    serialPort.open();

    if (0 == commandLineArgs['c'].compare("0"))
    {
        serialPort.enableDisableStreamOutput(false);
    }

    std::string reciveData = "";
    serialPort.read(reciveData);
    std::cout << "recieved data before erasing: " << reciveData << std::endl;
    Utility::eraseAllSubStrings(reciveData, kEraseString);
    std::cout << reciveData << std::endl;
    serialPort.close();
    return 0;

    serialPort.enableDisableStreamOutput(false);
    std::string stringRecieveData = "";
    for (int var = 0; var < 5; ++var)
    {
        serialPort.write("Hello ebi\r\n.Im from test program\r\n man ke khubam\r\n");
        std::string readedData = "";
        serialPort.read(readedData);
        stringRecieveData.append(readedData);
    }
    std::cout << stringRecieveData << std::endl;
    stringRecieveData = "";
    std::cout << "\n****************************************************" << std::endl;
    //serialPort.removeCrlfFromOutput();
    serialPort.enableDisableStreamOutput(true);
    for (int var = 0; var < 5; ++var)
    {
        serialPort.write("Hello ebi\r\n.Im from test program\r\n man ke khubam\r\n");
        std::string readedData = "";
        serialPort.read(readedData);
        stringRecieveData += readedData;
    }
    std::cout << stringRecieveData << std::endl;

    //sleep(10);
    //serialPort.read(readedData);
    //std::cout << readedData;
    serialPort.close();
    return 0;
}
