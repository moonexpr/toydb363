#include <string.h>

#ifndef H_ATTR
#include "Attribute.cpp"
#endif

#ifndef H_RECORD
#include "Record.cpp"
#endif

#ifndef H_LINKEDLIST
#include "../util/LinkedList.cpp"
#endif

#ifndef H_FILEMGNT
#include "../FileManager.cpp"
#endif

#define H_TABLE

class Table
{
    private:
    char * name;
    FileManager * fileManager;
    LinkedList * attributesTail;
    LinkedList * attributes;
    LinkedList * records;
    LinkedList * recordsTail;

    void load();

    public:
    Table(char * name);
    Table(char * fileName, FILE * fildHandle);
    ~Table();

    operator char *();
    std::ostream& operator << (std::ostream& outs);
    char * serialize();

    char * getName();
    LinkedList * getAttributes();
    void addAttribute(Attribute * attr);
    LinkedList * getRecords();
    void addRecord(Record * record);
    void save();
};

Table::Table(char * name)
{
    this->name = (char *) malloc(sizeof(char) * 50);
    memcpy(this->name, name, sizeof(char) * strlen(name) + 1);
    char * fileName = (char *) malloc(sizeof(char) * 100);
    sprintf(fileName, "%s.tb", name);

    this->fileManager = new FileManager(fileName);
    this->attributes = new LinkedList(0);
    this->attributesTail = new LinkedList(0);
    this->records = new LinkedList(0);
    this->recordsTail = new LinkedList(0);

    this->attributesTail->link(this->attributes, 0);
    this->recordsTail->link(this->records, 0);

    this->load();
}

Table::~Table()
{
    delete this->attributes;
}

Table::operator char *()
{
    char * format = (char *) malloc(sizeof(char) * 1024);
    char * format2 = (char *) malloc(sizeof(char) * 1024);

    if (!format) std::cerr << "malloc fail in table" << std::endl;

    unsigned counterRecords = 0;
    unsigned counterAttr = 0;
    LinkedList * nodeAttr = this->attributes;
    while (!(nodeAttr = nodeAttr->getNext())->isStructural())
    {
        counterAttr ++;
        sprintf(format2, "%s[%s]", format2, (char *) * static_cast<Attribute *>(nodeAttr->getData()));
    }


    LinkedList * nodeRecord = this->records;
    while (!(nodeRecord = nodeRecord->getNext())->isStructural()) counterRecords++;

    sprintf(format, "[%u]%s[%u]", counterAttr, format2, counterRecords);

    free(format2);

    return format;
}

std::ostream& Table::operator << (std::ostream& outs)
{
    return outs << (char *) *this;
}

void Table::load()
{
    Attribute ** attributes = this->fileManager->readAttributes();
    unsigned attributeLength = this->fileManager->getAttributeLength();
    Record ** records = this->fileManager->readRecords(attributeLength);
    int index = 0;
    while (records && records[index])
    {
        this->addRecord(records[index ++]);
    }

    index = 0;
    while (attributes && attributes[index])
    {
        this->addAttribute(attributes[index ++]);
    }
}

char * Table::serialize()
{
    char * format = (char *) malloc(sizeof(char) * 2048);

    sprintf(format, "%s", (char *) *this);

    LinkedList * nodeRecord = this->records;
    while (!(nodeRecord = nodeRecord->getNext())->isStructural())
    {
        Record * record = static_cast<Record *>(nodeRecord->getData());
        sprintf(format, "%s{%s}", format, (char *) *record);
    }

    return format;
}

char * Table::getName()
{
    return this->name;
}

LinkedList * Table::getAttributes()
{
    return this->attributes;
}

void Table::addAttribute(Attribute * attr)
{
    LinkedList * node = new LinkedList(attr);

    node->link(this->attributesTail->getPrev(), this->attributesTail);
}

LinkedList * Table::getRecords()
{
    return this->records;
}

void Table::addRecord(Record * record)
{
    LinkedList * node = new LinkedList(record);
    node->link(this->recordsTail->getPrev(), this->recordsTail);
}

void Table::save()
{
    char * data = this->serialize();
    this->fileManager->write(data);
    free(data);
}
