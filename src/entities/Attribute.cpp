#ifndef H_DT
#include "DataType.cpp"
#endif

#include <stdlib.h>
#include <iostream>
#include <cstring>

#define H_ATTR

class Attribute
{
    private:
    char * name;
    enum DataType datatype;

    public:

    Attribute(char * name, enum DataType dt);
    ~Attribute();

    operator char *();
    std::ostream& operator << (std::ostream& outs);

    const char * getName();
    void setName(char * name);

    enum DataType getDataType();
    void setDataType(enum DataType dt);
};
Attribute::Attribute(char * name, enum DataType dt)
{
    this->datatype 	= dt;
    this->name 	        = (char *) malloc(sizeof(char) * 50);

    memcpy(this->name, name, sizeof(char) * strlen(name) + 1);
}

Attribute::~Attribute()
{
}

Attribute::operator char *()
{

    char * format = (char *) malloc(sizeof(char) * 75);
    sprintf(format, "%s:%i", this->name, this->datatype);

    return format;
}

std::ostream& Attribute::operator << (std::ostream& outs)
{
    return outs << *this;
}

const char * Attribute::getName()
{
    return this->name;
}

void Attribute::setName(char * newName)
{
    this->name = newName;
}

enum DataType Attribute::getDataType() 
{
    return this->datatype;
}

void Attribute::setDataType(enum DataType dt)
{
    this->datatype = dt;
}
