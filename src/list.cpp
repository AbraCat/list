#include <stdlib.h>

#include <list.h>

const int max_elems = 10, poison_val = -1;

ErrEnum listCtor(List* list)
{
    list->data = (ListElem*)calloc(max_elems, sizeof(ListElem));
    if (list->data == NULL) return ERR_MEM;
    list->next = (int*)calloc(max_elems * 2, sizeof(int));
    if (list->next == NULL) return ERR_MEM;
    list->prev = list->next + max_elems;
    list->head = 0;
    list->tail = 0;
    list->free = 1;

    for (int i = 0; i < max_elems - 1; ++i)
    {
        list->data[i] = list->prev[i] = poison_val;
        list->next[i] = i + 1;
    }
    list->data[max_elems - 1] = list->prev[max_elems - 1] = poison_val;
    list->next[max_elems - 1] = 0;

    return ERR_OK;
}

void listDtor(List* list)
{
    free(list->data);
    free(list->next);
}
ErrEnum listVerify(List* list)
{
    if (list == NULL) return ERR_OK; // not ok

    return ERR_OK;
}
void listDump(FILE* fout, List* list)
{
    if (fout == NULL || list == NULL) return;

    fprintf(fout, "List [0x%p]\n\nhead: %d\ntail: %d\nfree: %d\n\n", 
    list, list->head, list->tail, list->free);

    fprintf(fout, "\ndata [0x%p]: ", list->data);
    for (int i = 0; i < max_elems; ++i)
        fprintf(fout, "%d ", list->data[i]);
    fprintf(fout, "\nnext [0x%p]: ", list->next);
    for (int i = 0; i < max_elems; ++i)
        fprintf(fout, "%d ", list->next[i]);
    fprintf(fout, "\nprev [0x%p]: ", list->prev);
    for (int i = 0; i < max_elems; ++i)
        fprintf(fout, "%d ", list->prev[i]);
    fputs("\n\n", fout);
}

ErrEnum listGetFront(List* list, ListElem** elem)
{
    if (list == NULL || elem == NULL) return ERR_OK; //
    if (list->head == 0)
    {
        *elem = NULL;
        return ERR_OK; //
    }
    *elem = list->data + list->head;
    return ERR_OK;
}

ErrEnum listGetBack(List* list, ListElem** elem)
{
    if (list == NULL || elem == NULL) return ERR_OK; //
    if (list->tail == 0)
    {
        *elem = NULL;
        return ERR_OK; //
    }
    *elem = list->data + list->tail;
    return ERR_OK;
}

ErrEnum listNext(List* list, ListElem* elem, ListElem** ans)
{
    if (list == NULL || elem == NULL || ans == 0) return ERR_OK; //

    int i = elem - list->data;
    // check for i bound

    if (list->data[i] == poison_val) return ERR_OK; //
    if (list->next[i] == 0)
    {
        *ans = NULL;
        return ERR_OK; //
    }
    *ans = list->data + list->next[i];
    return ERR_OK;
}
ErrEnum listPrev(List* list, ListElem* elem, ListElem** ans)
{
    if (list == NULL || elem == NULL || ans == 0) return ERR_OK; //
    int i = elem - list->data;
    // check for i bound
    if (list->data[i] == poison_val) return ERR_OK; //
    if (list->prev[i] == 0)
    {
        *ans = NULL;
        return ERR_OK; //
    }
    *ans = list->data + list->prev[i];
    return ERR_OK;
}

ErrEnum listPushFront(List* list, ListElem val)
{
    // err check

    // check adding elem to last free cell

    if (list->free == 0) return ERR_OK; //

    int new_free = list->next[list->free];

    list->data[list->free] = val;
    list->next[list->free] = list->head;
    list->prev[list->free] = 0;
    if (list->head == 0) list->tail = list->free;
    else list->prev[list->head] = list->free;
    list->head = list->free;
    list->free = new_free;

    return ERR_OK;
}

ErrEnum listPushBack(List* list, ListElem val)
{
    // err check

    if (list->free == 0) return ERR_OK; //

    int new_free = list->next[list->free];

    list->data[list->free] = val;
    list->prev[list->free] = list->tail;
    list->next[list->free] = 0;
    if (list->head == 0) list->head = list->free;
    else list->next[list->tail] = list->free;
    list->tail = list->free;
    list->free = new_free;

    return ERR_OK;
}

ErrEnum listPopFront(List* list, ListElem* val)
{
    // err check
    if (list->head == 0) return ERR_OK; //

    int new_head = list->next[list->head];
    if (val != NULL) *val = list->data[list->head];

    list->data[list->head] = poison_val;
    list->next[list->head] = list->free;
    list->prev[list->head] = poison_val;
    if (list->head == list->tail) list->tail = 0;
    else list->prev[new_head] = 0;
    list->free = list->head;
    list->head = new_head;

    return ERR_OK;
}
ErrEnum listPopBack(List* list, ListElem* val)
{
    // err check
    if (list->head == 0) return ERR_OK; 
    
    int new_tail = list->prev[list->tail];
    if (val != NULL) *val = list->data[list->tail];

    list->data[list->tail] = poison_val;
    list->prev[list->tail] = poison_val;
    list->next[list->tail] = list->free;
    if (list->head == list->tail) list->head = 0;
    else list->next[new_tail] = 0;
    list->free = list->tail;
    list->tail = new_tail;

    return ERR_OK;
}

ErrEnum listInsertBefore(List* list, ListElem* elem, ListElem val)
{
    // err check
    if (list->free == 0) return ERR_OK; //

    int elem_ind = elem - list->data;
    // check for elem_ind bound
    if (list->data[elem_ind] == poison_val) return ERR_OK; //
    int new_free = list->next[list->free];

    list->data[list->free] = val;
    list->next[list->free] = elem_ind;
    if (list->head == elem_ind) list->head = list->free;
    else list->next[list->prev[elem_ind]] = list->free;
    list->prev[list->free] = list->prev[elem_ind];
    list->prev[elem_ind] = list->free;
    list->free = new_free;

    return ERR_OK;
}

ErrEnum listInsertAfter(List* list, ListElem* elem, ListElem val)
{
    // err check
    if (list->free == 0) return ERR_OK; //

    int elem_ind = elem - list->data;
    // check for elem_ind bound
    if (list->data[elem_ind] == poison_val) return ERR_OK; //
    int new_free = list->next[list->free];

    list->data[list->free] = val;
    list->prev[list->free] = elem_ind;
    if (list->tail == elem_ind) list->tail = list->free;
    else list->prev[list->next[elem_ind]] = list->free;
    list->next[list->free] = list->next[elem_ind];
    list->next[elem_ind] = list->free;
    list->free = new_free;

    return ERR_OK;
}

ErrEnum listErase(List* list, ListElem* elem)
{
    // err check
    
    int elem_ind = elem - list->data;
    // check for elem_ind bound
    if (list->data[elem_ind] == poison_val) return ERR_OK; //

    list->data[elem_ind] = poison_val;
    if (list->head == elem_ind) list->head = list->next[elem_ind];
    else list->next[list->prev[elem_ind]] = list->next[elem_ind];
    if (list->tail == elem_ind) list->tail = list->prev[elem_ind];
    else list->prev[list->next[elem_ind]] = list->prev[elem_ind];
    list->next[elem_ind] = list->free;
    list->prev[elem_ind] = poison_val;
    list->free = elem_ind;

    return ERR_OK;
}

ErrEnum listByVal(List* list, ListElem val, ListElem** ans)
{
    // err check

    for (int i = list->head; i != 0; i = list->next[i])
    {
        if (list->data[i] == val)
        {
            *ans = list->data + i;
            return ERR_OK;
        }
    }
    *ans = NULL;
    return ERR_OK;
}

ErrEnum listByIndex(List* list, int ind, ListElem** ans)
{
    // err check

    int i = list->head;
    for (int cnt = 0; cnt != ind; ++cnt)
    {
        if (i == 0)
        {
            *ans = NULL;
            return ERR_OK; //
        }
        i = list->next[i];
    }
    if (i == 0)
    {
        *ans = NULL;
        return ERR_OK; //
    }
    *ans = list->data + i;
    return ERR_OK;
}