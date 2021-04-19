#include <string.h>
#include <string>
#include <iostream>
#include <stdlib.h>

#define H_UTIL_STRING

namespace UtilString {
    char * getFirstWord(char * string, int length);
    char * getFirstWordDelim(char * string, int length, char delimiter);
    char * getWord(char * string, int length, int index);
    char * getWordDelim(char * string, int length, int index, char delimiter);
    void trim(char * string);
};


void UtilString::trim(char * string)
{
    std::string str = string;

    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
        return;

    size_t last = str.find_last_not_of(' ');
    std::string newStr = str.substr(first, (last - first + 1));
    memcpy(string, newStr.c_str(), sizeof(char) * newStr.length() + 1);
}

char * UtilString::getWord(char * string, int length, int index) {
    return UtilString::getWordDelim(string, length, index, ' ');
}

char * UtilString::getWordDelim(char * string, int length, int index, char delimiter) {
    char * cpyString = (char *) malloc(sizeof(char) * length);
    strcpy(cpyString, string);

    int cIndex = 0;
    int c;
    for (c = 0; c < length; c ++)
    {
        if (cIndex == index)
            break;

        if (string[c] == delimiter)
            cIndex++;
    }

    if (cIndex < index)
    {
        return 0;
    }

    int cIndexEnd = c;
    while(cIndexEnd < length && string[cIndexEnd] != delimiter)
        cIndexEnd++;

    cpyString[cIndexEnd] = 0;

    return &cpyString[c];
}

char * UtilString::getFirstWord(char * string, int length) {
    return UtilString::getFirstWordDelim(string, length, ' ');
}

char * UtilString::getFirstWordDelim(char * string, int length, char delimiter) {
    char * cpyString = (char *) malloc(sizeof(char) * length);
    strcpy(cpyString, string);

    int c = 0;
    while(c < length && string[c] != delimiter)
        c++;

    cpyString[c] = 0;

    return cpyString;
}
