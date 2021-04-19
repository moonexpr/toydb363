#include <map>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef H_ATTR
#include "entities/Attribute.cpp"
#endif

#ifndef H_RECORD
#include "entities/Record.cpp"
#endif


class FileManager
{
    private:
    FILE * fileHandle;
    char * fileName;
    char * fileBuffer;
    int fileLength;


    public:
    FileManager(char * fileName);
    ~FileManager();

    char * getFileName();

    char * readBuffer(char * fileName);
    unsigned getHeaderOffset(int offset);
    unsigned getBodyOffset(int offset);
    unsigned getAttributeLength();
    Record ** readRecords(unsigned attrLength);
    Attribute ** readAttributes();
    Record * readSingleRecord(char * content, unsigned length, unsigned attrLength);
    Attribute * readSingleAttribute(char * content, int length);
    unsigned getAttributesOffset();
    char * substring(char * string, int start, int length);

    void write(char * content);
};

FileManager::FileManager(char * fileName)
{
    this->fileName = fileName;
    this->fileHandle = fopen(fileName, "r");
    this->fileBuffer = this->readBuffer(fileName);

}

FileManager::~FileManager()
{
    free(this->fileName);
    free(this->fileBuffer);
    fclose(this->fileHandle);
}

char * FileManager::readBuffer(char * fileName)
{
    if (!this->fileHandle)
        return 0;

    fseek(this->fileHandle, 0, SEEK_END);
    this->fileLength = ftell(this->fileHandle);
    rewind(this->fileHandle);

    char * buff = (char *) malloc(sizeof(char) * this->fileLength);
    fread(buff, sizeof(char), this->fileLength, this->fileHandle);

    rewind(this->fileHandle);

    return buff;
}

unsigned FileManager::getAttributesOffset()
{
    unsigned start = 0;
    unsigned end = this->getHeaderOffset(0);
    unsigned count = std::stoi(this->substring(this->fileBuffer, start + 1, end - 1));
    
    Attribute ** attrArr = (Attribute **) malloc(sizeof(Attribute **) * count);

    for (int index = 0; index < count; index++)
    {
        start = end + 1;
        end = this->getHeaderOffset(start);
    }

    return end + 1;
}

Record ** FileManager::readRecords(unsigned attrLength)
{
    if (!this->fileBuffer)
        return 0;

    unsigned start = this->getAttributesOffset();
    unsigned end = this->getHeaderOffset(start);
    unsigned count = std::atoi(this->substring(this->fileBuffer, start + 1, end - start - 1));

    Record ** records = (Record **) malloc(sizeof(Record *) * sizeof(count));

    for (int index = 0; index < count; index ++)
    {
        start = end + 1;
        end = this->getBodyOffset(start);
        char * attrContent = this->substring(this->fileBuffer, start + 1, end - start - 1);
        records[index] = this->readSingleRecord(attrContent, strlen(attrContent), attrLength);
    }

    return records;
}

Record * FileManager::readSingleRecord(char * content, unsigned length, unsigned attrLength)
{
    char ** values = (char **) malloc(sizeof(char *) * attrLength);
    bool escape;
    int index;
    int lastIndex = 0;
    int attrIndex = 0;

    for (index = 0; index < length; index ++)
    {
        if (content[index] == '\\')                 escape = true;

        if (content[index] == '|' && !escape)
        {
            values[attrIndex ++] = this->substring(content, lastIndex, index - lastIndex);
            lastIndex = index + 1;
        }

        if (escape)                                escape = false;
    }

    values[attrIndex] = this->substring(content, lastIndex, index - lastIndex);

    Record * record = new Record();

    for (index = 0; index < attrLength; index ++)
    {
        record->addValue(values[index]);
    }

    return record;
}

Attribute ** FileManager::readAttributes()
{
    if (!this->fileBuffer)
        return 0;

    unsigned start = 0;
    unsigned end = this->getHeaderOffset(0);
    unsigned count = std::stoi(this->substring(this->fileBuffer, start + 1, end - 1));
    
    Attribute ** attrArr = (Attribute **) malloc(sizeof(Attribute *) * count);

    for (int index = 0; index < count; index++)
    {
        start = end + 1;
        end = this->getHeaderOffset(start);
        char * attrContent = this->substring(this->fileBuffer, start + 1, end - start - 1);
        attrArr[index] = this->readSingleAttribute(attrContent, strlen(attrContent));
    }

    return attrArr;
}

Attribute * FileManager::readSingleAttribute(char * content, int length)
{
    bool escape;
    int index;
    for (index = 0; index < length; index ++)
    {
        if (content[index] == ':' && !escape)      break;
        if (this->fileBuffer[index] == '\\')       escape = true;
        if (escape)                                escape = false;
    }

    char * name     = this->substring(content, 0, index);
    int datatype    = std::atoi(this->substring(content, index + 1, length - index - 1));

    return new Attribute(name, static_cast<enum DataType>(datatype));
}

unsigned FileManager::getHeaderOffset(int offset)
{
    int headerState = 0;
    int index;
    for (index = offset; index < this->fileLength; index ++)
    {
        bool escape = (headerState & 0x8) > 0;

        if (this->fileBuffer[index] == '[' && !escape)      headerState |= 0x1;
        if (this->fileBuffer[index] == ']' && !escape)      headerState |= 0x2;
        if (this->fileBuffer[index] == '\\')                headerState |= 0x4;
        if (escape)                                         headerState &= 0x3;

        if (headerState == 0x3)
            break;

    }

    return index;
}

unsigned FileManager::getAttributeLength()
{
    if (!this->fileBuffer)
        return 0;

    unsigned start = 0;
    unsigned end = this->getHeaderOffset(0);
    return std::stoi(this->substring(this->fileBuffer, start + 1, end - 1));
}

unsigned FileManager::getBodyOffset(int offset)
{
    int headerState = 0;
    int index;
    for (index = offset; index < this->fileLength; index ++)
    {
        bool escape = (headerState & 0x8) > 0;

        if (this->fileBuffer[index] == '{' && !escape)      headerState |= 0x1;
        if (this->fileBuffer[index] == '}' && !escape)      headerState |= 0x2;
        if (this->fileBuffer[index] == '\\')                headerState |= 0x4;
        if (escape)                                         headerState &= 0x3;

        if (headerState == 0x3)
            break;

    }

    return index;
}

char * FileManager::substring(char * string, int start, int length)
{
    char * substr = (char *) malloc(length);
    std::string str = string;
    memcpy(substr, str.substr(start, length).c_str(), length);
    substr[length] = 0;

    return substr;
}

char * FileManager::getFileName()
{
    return fileName;
}

void FileManager::write(char * content)
{
    FILE * file = fopen(this->fileName, "wb");
    fputs(content, file);
    fflush(file);
    fclose(file);
}
