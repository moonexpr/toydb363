#include <iostream>
#include <string.h>

#ifndef H_LINKEDLIST
#include "../util/LinkedList.cpp"
#endif

#define H_RECORD

class Record 
{
    private:
    LinkedList * values;
    LinkedList * valuesTail;
    LinkedList * getNode(int index);

    public:
    Record();
    ~Record();

    operator char *();
    std::ostream& operator << (std::ostream& outs);

    void setValue(int index, char * data);
    void addValue(char * data);
    char * getValue(int index);
};

Record::Record()
{
    this->values = new LinkedList(0);
    this->valuesTail = new LinkedList(0);

    this->valuesTail->link(this->values, 0);
}

Record::~Record()
{
    delete this->values;
}

Record::operator char *()
{
    char * format = (char *) malloc(sizeof(char) * 1024);

    LinkedList * node = this->values;
    while (!(node = node->getNext())->isStructural())
    {
        sprintf (
            format,
            ! node->getNext()->isStructural() ? "%s%s|" : "%s%s",
            format,
            static_cast<char *>(node->getData())
        );
    }

    return format;
}

std::ostream& Record::operator << (std::ostream& outs)
{
    return outs << (char *) *this;
}


LinkedList * Record::getNode(int index)
{
    int nodeIndex = 0;
    LinkedList * node = this->values;
    
    while ((node = node->getNext()))
        if (nodeIndex ++ == index) break;
    
    if (!node) throw "Attempting to non-existant value in record."; 

    return node;
}

void Record::setValue(int index, char * data)
{
    LinkedList * node = this->getNode(index);
    char * prevData = static_cast<char *>(node->getData());
    memcpy(prevData, data, sizeof(data) + 1);
}


void Record::addValue(char * data)
{
    // Perform a deep copy of values so
    // we don't have to deal with pointers.
    char * newData = (char *) malloc(sizeof(data));
    memcpy(newData, data, sizeof(data) + 1);
    
    LinkedList * newNode = new LinkedList(newData);
    newNode->link(this->valuesTail->getPrev(), this->valuesTail);
}

char * Record::getValue(int index)
{
    return static_cast<char *>(this->getNode(index)->getData());
}

