#include <stdlib.h>

#define H_LINKEDLIST

class LinkedList {
    private:
    void * data;
    LinkedList * next;
    LinkedList * prev;

    public:

    LinkedList(void * data);
    ~LinkedList();

    void link(LinkedList * prev, LinkedList * next);
    void unlink();

    void * getData();
    LinkedList * getNext();
    LinkedList * getPrev();
    bool isStructural();
    void setNext(LinkedList * next);
    void setPrev(LinkedList * prev);
};


LinkedList::LinkedList(void * data)
{
    this->data = data;
    this->prev = 0;
    this->next = 0;
}

LinkedList::~LinkedList()
{
    free(this->data);

    if (this->next)
        delete this->next;
}

void LinkedList::unlink()
{
    if (this->prev)
        this->prev->setNext(this->next);
    

    if (this->next)
        this->next->setPrev(this->prev);

    this->prev = 0;
    this->next = 0;
}

void LinkedList::link(LinkedList * prev, LinkedList * next)
{
    if (prev)
    {
        prev->setNext(this);
        this->prev = prev;
    }

    if (next)
    {
        next->setPrev(this);
        this->next = next;
    }
}

void * LinkedList::getData()
{
    return this->data;
}

LinkedList * LinkedList::getNext()
{
    return this->next;
}

LinkedList * LinkedList::getPrev()
{
    return this->prev;
}

bool LinkedList::isStructural()
{
    return (!this->prev || !this->next) && !this->data;
}

void LinkedList::setNext(LinkedList * next)
{
    this->next = next;
}

void LinkedList::setPrev(LinkedList * prev)
{
    this->prev = prev;
}
