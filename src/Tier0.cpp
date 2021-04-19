#include <iostream>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef H_TABLE
#include "entities/Table.cpp"
#endif

#ifndef H_ATTR
#include "entities/Attribute.cpp"
#endif

#ifndef H_RECORD
#include "entities/Record.cpp"
#endif

#ifndef H_DT
#include "entities/DataType.cpp"
#endif

#ifndef H_UTIL_STRING
#include "util/String.cpp"
#endif

#define H_TIER0
#define MAX_RESPONSE 60

enum ResultCode {
    Continue	    = 0,
    GraceExit	    = 2 << 1,
    Unimplemented   = 2 << 2,
};

class Tier0
{
    private:
    std::map<std::string, Table *> tables;
    void prompt(const char * statement, char * responseBuffer);
    enum DataType datatype(char * type);

    public:
    Tier0();
    ~Tier0();
    
    char * getFileName(char * tableName);
    char * escape(char * input, int length);

    Table * findTable(char * tableName);

    void validateInput(char * input, enum DataType dt);
    void dumpRecord(Table * table, Record * record);

    int actionCreate(char * input, int length);
    int actionHeader(char * input, int length);
    int actionInsert(char * input, int length);
    int actionDisplay(char * input, int length);
    int actionDelete(char * input, int length);
    int actionSearch(char * input, int length);
    int actionQuit(char * input, int length);
};

Tier0::Tier0()
{
}

Tier0::~Tier0()
{
    std::map<std::string, Table *>::iterator iter;
    for (iter = tables.begin(); iter != tables.end(); iter ++)
    {
        delete iter->second;
    }
}

enum DataType Tier0::datatype(char * type)
{
    int numericType = type[0] - '0';
    if (numericType > 4 || numericType < 0) {
        throw "bad type";
    }
    
    return (enum DataType) numericType;
}

void Tier0::prompt(const char * statement, char * responseBuffer)
{
    std::cout << statement;
    std::cin.getline(responseBuffer, MAX_RESPONSE);
}

char * Tier0::getFileName(char * tableName)
{
    char * fileName = (char *) malloc(sizeof(char) * 100);
    sprintf(fileName, "%s.tb", tableName);

    return fileName;
}

Table * Tier0::findTable(char * tableName)
{
    char * fileName = this->getFileName(tableName);
    struct stat buff;
    if (stat(fileName, &buff)) throw "The specified table does not exist.";

    return new Table(tableName);
}

int Tier0::actionCreate(char * input, int length)
{
    char * tableName;
    if (!(tableName = UtilString::getWord(input, length, 1)))
        throw "[usage] Toy create (table name)";

    Table * table = new Table(tableName);

    char * name = (char *) malloc(sizeof(char) * MAX_RESPONSE);
    char * type = (char *) malloc(sizeof(char) * MAX_RESPONSE);

    if (!name || !type) throw "malloc fail in actionCreate!";
    
    do {
        Tier0::prompt("Attribute name: ", name);
        Tier0::prompt("Select a type: 1. integer; 2. double; 3. boolean; 4. string: ", type);

        table->addAttribute(new Attribute(name, this->datatype(type)));

        Tier0::prompt("More attribute? (y/n) ", name);


    } while (name[0] == 'y');

    table->save();

    free(name);
    free(type);

    return Continue;
}

int Tier0::actionHeader(char * input, int length)
{
    char * tableName;
    if (!(tableName = UtilString::getWord(input, length, 1)))
        throw "[usage] Toy header (table name)";

    Table * table = this->findTable(tableName);
    
    unsigned counterAttr = 0;
    LinkedList * node = table->getAttributes();
    while (!(node = node->getNext())->isStructural()) { 
        counterAttr ++;
        Attribute * attr = static_cast<Attribute *>(node->getData());
        std::cout << "Attribute " << counterAttr << ": " << attr->getName() << std::endl;
    }

    std::cout << counterAttr << " attributes" << std::endl;
    std::cout << "0 records" << std::endl;

    delete table;

    return Continue;
}

void Tier0::validateInput(char * input, enum DataType dt)
{
    int points = 0;
    for (int i = 0; i < strlen(input); i++)
    {
        char c = input[i];
        switch(dt)
        {
            case DT_String:
                break;

            case DT_Double:
                std::cout << c << std::endl;
                if ((c < '0' || c > '9') && c != '.')   throw "Bad input, expected a decimal number.";
                if (c == '.')                           points ++;
                break;

            case DT_Boolean:
                if (c != 'T' || c != 'F')               throw "Bad input, expected either T, or F.";
                break;

            case DT_Integer:
                if (c < '0' || c > '9')                 throw "Bad input, expected an integer.";
                break;
        }
    }

    if (points != 1 && dt == DT_Double)
        throw "Bad input, expecting a decimal number.";

}

int Tier0::actionInsert(char * input, int length)
{
    char * tableName;
    if (!(tableName = UtilString::getWord(input, length, 1)))
        throw "[usage] Toy insert (table name)";

    Table * table = this->findTable(tableName);

    char * questionBuffer = (char *) malloc(sizeof(char) * MAX_RESPONSE);
    char * responseBuffer = (char *) malloc(sizeof(char) * MAX_RESPONSE);

    LinkedList * node = table->getAttributes();
    Record * newRecord = new Record();

    while (! (node->getNext())->isStructural()) { 
        node = node->getNext();
        Attribute * attr = static_cast<Attribute *>(node->getData());
        sprintf(questionBuffer, "%s: ", attr->getName());
        
        this->prompt(questionBuffer, responseBuffer);

        this->validateInput(responseBuffer, attr->getDataType());

        newRecord->addValue(responseBuffer);
    } 

    table->addRecord(newRecord);

    table->save();

    free(questionBuffer);
    free(responseBuffer);

    return Continue;
}

void Tier0::dumpRecord(Table * table, Record * record)
{
    LinkedList * attrNode = table->getAttributes();

    int index = 0;
    while (! (attrNode = attrNode->getNext())->isStructural())
    {
        Attribute * attr = static_cast<Attribute *>(attrNode->getData());
        std::cout << attr->getName() << ": " << record->getValue(index ++) << std::endl;
    }
}

int Tier0::actionDisplay(char * input, int length)
{
    int targetId;
    char * tableName;

    if (!(UtilString::getWord(input, length, 1)))
        throw "[usage] Toy display (record id) (table name)";

    if (!(tableName = UtilString::getWord(input, length, 2)))
        throw "[usage] Toy display (record id) (table name)";

    targetId = atoi(UtilString::getWord(input, length, 1));

    Table * table = this->findTable(tableName);

    int recordId = 0;
    LinkedList * recNode = table->getRecords();
    while (! (recNode = recNode->getNext())->isStructural())
        if (recordId ++ == targetId)
            break;

    if (recNode->isStructural()) throw "This record does not exist.";

    this->dumpRecord(table, static_cast<Record *>(recNode->getData()));

    return Continue;
}

int Tier0::actionDelete(char * input, int length)
{
    int targetId;
    char * tableName;

    if (!(UtilString::getWord(input, length, 1)))
        throw "[usage] Toy delete (record id) (table name)";

    if (!(tableName = UtilString::getWord(input, length, 2)))
        throw "[usage] Toy delete (record id) (table name)";

    targetId = atoi(UtilString::getWord(input, length, 1));
    Table * table = this->findTable(tableName);

    int recId = 0;
    LinkedList * node = table->getRecords();

    while (! (node = node->getNext())->isStructural())
        if (recId ++ == targetId) break;

    if (node->isStructural()) throw "This record does not exist.";

    node->unlink();
    delete node;

    table->save();

    return Continue;
}

int Tier0::actionSearch(char * input, int length)
{
    char * condition;
    char * tableName;
    if (!(condition = UtilString::getWordDelim(input, length, 1, '"')))
        throw "[usage] Toy search \"(condition)\" (table name)";

    if (!(tableName = UtilString::getWordDelim(input, length, 2, '"')))
        throw "[usage] Toy search \"(condition)\" (table name)";
    
    char * haystack     = UtilString::getWordDelim(condition, length, 0, '=');
    char * needle       = UtilString::getWordDelim(condition, length, 1, '=');

    UtilString::trim(tableName);
    UtilString::trim(haystack);
    UtilString::trim(needle);

    Table * table = this->findTable(tableName);

    int attrIndex = -1;
    int index = 0;
    LinkedList * nodeAttr = table->getAttributes();
    while (! (nodeAttr = nodeAttr->getNext())->isStructural())
    {
        index ++;
        Attribute * attr = static_cast<Attribute *>(nodeAttr->getData());

        if (strcmp(attr->getName(), haystack))
        {
            attrIndex = index;
            break;
        }
    }

    if (attrIndex == -1) throw "This attribute does not exist.";

    LinkedList * nodeRecord = table->getRecords();
    index = 0;
    while (! (nodeRecord = nodeRecord->getNext())->isStructural())
    {
        index ++;
        Record * record = static_cast<Record *>(nodeRecord->getData());
        if (!strcmp(record->getValue(attrIndex), needle))
        {
            std::cout << "Record " << index << std::endl;
            this->dumpRecord(table, record);
        }
    }

    return Continue;
}

int Tier0::actionQuit(char * input, int length)
{
    return GraceExit;
}
