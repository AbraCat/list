#ifndef LIST_H
#define LIST_H

#include <stdio.h>

#include <error.h>

typedef int ListElem;

struct List
{
    ListElem *data;
    int *next, *prev;
    int free;
};

ErrEnum listCtor(List* list);
void listDtor(List* list);
ErrEnum listVerify(List* list);
void listDump(FILE* fout, List* list);

ErrEnum listGetFront(List* list, ListElem** elem);
ErrEnum listGetBack(List* list, ListElem** elem);

ErrEnum listNext(List* list, ListElem* elem, ListElem** ans);
ErrEnum listPrev(List* list, ListElem* elem, ListElem** ans);

ErrEnum listPushFront(List* list, ListElem val);
ErrEnum listPushBack(List* list, ListElem val);
ErrEnum listPopFront(List* list, ListElem* val);
ErrEnum listPopBack(List* list, ListElem* val);

ErrEnum listInsertBefore(List* list, ListElem* elem, ListElem val);
ErrEnum listInsertAfter(List* list, ListElem* elem, ListElem val);
ErrEnum listErase(List* list, ListElem* elem);

ErrEnum listByVal(List* list, ListElem val, ListElem** ans);
ErrEnum listByIndex(List* list, int ind, ListElem** ans);

#endif // LIST_H