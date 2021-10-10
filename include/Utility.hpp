// Header guard
#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <unordered_map>

class Utility
{
public:
    ///
    /// \brief Is used to parse comman line arguments
    /// \param argc: Number of items in argv array
    /// \param argv: list of command line arguments
    /// \param optionList: Result of parsing arguments will be put in this
    ///     map
    /// \return true in success and false in fail.
    ///
    bool static parseCommandLineArgs(int argc, char *argv[],
                   std::unordered_map<char, std::string> &optionList);
    //-----------------------------------------------------------------------
    ///
    /// \brief Is used to print about text.
    ///
    void static printAboutText();
    //-----------------------------------------------------------------------
    ///
    /// \brief Erase all Occurrences of given substring from main string.
    ///
    /// \param mainString
    /// \param stringToErase
    ///
    void static eraseAllSubStrings(
            std::string &mainString,
            const std::string &stringToErase);
};

#endif // #ifndef SERIAL_PORT_SERIAL_PORT_H
