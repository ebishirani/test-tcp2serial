#include<iostream>
#include <unistd.h>
#include <cstdlib>
#include <unordered_map>
#include <sstream>
#include <csignal>

#include "SerialPort.hpp"
#include "Exception.hpp"
#include "Utility.hpp"
#include "SyncQueue.hpp"
#include "TcpServer.h"

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

bool mustInterpretRuntimeOptions = false;
//---------------------------------------------------------------------------
// on sig_exit, close client
void exitSignal(int s)
{
    mustTerminateApp = true;
    commonBuffer.enQueue("");
}
//---------------------------------------------------------------------------
void handleRecievedDataFromTcpPort(SerialPort &serialPort)
{
    //Sometimes it is pissible that a command and it's parameters are placed
    // in tow consecutiuve packets.In this situation we hold current section
    // of command in next strig.
    std::string dataFromPrePacket("");
    std::string currentPacket("");

    while (false == mustTerminateApp)
    {
        currentPacket.clear();
        commonBuffer.deQueueWithTimeOut(1500, currentPacket);
        if (true == mustTerminateApp)
        {
            break;
        }

        if (false == mustInterpretRuntimeOptions)
        {
            if (currentPacket.length() > 0)
            {
                if (true == mustBeRemoveCrlf)
                {
                    Utility::eraseAllSubStrings(currentPacket, kEraseString);
                }
                serialPort.write(currentPacket);
            }
        }
        else
        {
            if (dataFromPrePacket.length() > 0)
            {
                currentPacket.insert(0, dataFromPrePacket);
                dataFromPrePacket.clear();
            }

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
                else if (currentPacket.length() < 2)
                {
                    dataFromPrePacket = currentPacket;
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
                        if(currentPacket.length() > 1 &&
                                0 != currentPacket.compare(0, 2, "\r\n", 2))
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
                            //We erase 3 characters to delet 1 spaces before
                            //and 1 space after option value
                            currentPacket.erase(0, 3);
                            continue;
                        }
                        else
                        {
                            if (0 == currentPacket.compare(0 ,1, "\n", 1))
                            {
                                currentPacket.erase(0, 1);
                            }
                            else if (0 == currentPacket.compare(0, 1, "\r", 1))
                            {
                                currentPacket.erase(0, 1);
                            }
                            else if (0 == currentPacket.compare(0, 2, "\n\r", 2))
                            {
                                currentPacket.erase(0, 2);
                            }
                            dataFromPrePacket = command + currentPacket;
                            break;
                        }
                    }
                    //-----------------------------------------------------------
                    else if (0 == command.compare("-c"))
                    {
                        currentPacket.erase(0, 2);
                        if(currentPacket.length() > 1 &&
                                0 != currentPacket.compare(0, 2, "\r\n", 2))
                        {
                            std::string::size_type spacePosition =
                                    currentPacket.find(' ', 1);

                            std::string commandType =
                                    currentPacket.substr(1, spacePosition - 1);

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
                            if (0 == currentPacket.compare(0 ,1, "\n", 1))
                            {
                                currentPacket.erase(0, 1);
                            }
                            else if (0 == currentPacket.compare(0, 1, "\r", 1))
                            {
                                currentPacket.erase(0, 1);
                            }
                            else if (0 == currentPacket.compare(0, 2, "\n\r", 2))
                            {
                                currentPacket.erase(0, 2);
                            }
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
                    else
                    {
                        serialPort.write(command);
                        currentPacket.erase(0, 2);
                        continue;
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
                        //dashPosition--;
                        std::string dataToSend =
                                currentPacket.substr(0, dashPosition);
                        if (true == mustBeRemoveCrlf)
                        {
                            Utility::eraseAllSubStrings(dataToSend,
                                                        kEraseString);
                        }
                        serialPort.write(dataToSend);
                        currentPacket.erase(0, dashPosition);
                        continue;
                    }
                }

            }//End of while(tru).
        }
    }//End of while (false == mustTerminateApp).
}
//---------------------------------------------------------------------------
// observer callback. will be called for every new message received from tcp
// socket
void onClientMsgReceived(
        const std::string &clientIP,
        const char *msg,
        size_t size)
{
    std::string msgStr(msg);
    // Register received message to process.
    commonBuffer.enQueue(msgStr);
}
//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    //register to SIGINT to close program when user press ctrl+c
    signal(SIGINT, exitSignal);

    std::unordered_map<char, std::string> commandLineArgs;

    bool parseResult = Utility::parseCommandLineArgs(argc, argv,
                                                     commandLineArgs);

    if (false == parseResult)
    {
        return 0;
    }

    if (0 == commandLineArgs['d'].compare("1"))
    {
        mustInterpretRuntimeOptions = true;
    }

    unsigned int boudrate = std::stoul(commandLineArgs['b'], nullptr, 0);
    SerialPort serialPort(commandLineArgs['s'], boudrate);

    if (0 == commandLineArgs['o'].compare("1"))
    {
        mustBeRemoveCrlf = true;
    }
    //SerialPort serialPort("/dev/ttyS1", boudrate);
    try
    {
        serialPort.open();
    }
    catch(Exception e)
    {
        std::cout << e.what() << std::endl;
        return 0;
    }
    if (0 == commandLineArgs['c'].compare("0"))
    {
        serialPort.enableDisableStreamOutput(false);
    }
    //try to create and start tcp server
    unsigned int serverPortNumber =
            std::stoul(commandLineArgs['p'], nullptr, 0);
    TcpServer tcpServer;
    PipeRet startRet = tcpServer.start(serverPortNumber);

    if (startRet.isSuccessful())
    {
        std::cout << "Server setup succeeded\n";
    }
    else
    {
        std::cout << "Server setup failed: " << startRet.message() << "\n";
        serialPort.close();
        return 0;
    }
    ServerObserver observer;
    // configure and register observer
    observer.incomingPacketHandler = onClientMsgReceived;
    observer.wantedIP = "";
    tcpServer.subscribe(observer);

    while (true)
    {
        try
        {
            std::cout << "waiting for incoming client...\n";
            std::string clientIP = tcpServer.acceptClient(0);
            std::cout << "accepted new client with IP: " << clientIP << "\n" <<
                         "== updated list of accepted clients ==" << "\n";
            tcpServer.printClients();
            break;
        }
        catch(const std::runtime_error &error)
        {
            std::cout << "Accepting client failed: " << error.what() << "\n";
        }
    }

    handleRecievedDataFromTcpPort(serialPort);

    tcpServer.close();
    serialPort.close();

    return 0;
}
