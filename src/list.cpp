#include <stdlib.h>

#include <list.h>

const char log_path[] = "./log"; // relative to directory where exe runs
const int max_elems = 10, poison_val = 44203, buffer_size = 500;
int dump_cnt = 0;

ErrEnum listCtor(List* list)
{
    list->data = (ListElem*)calloc(max_elems, sizeof(ListElem));
    if (list->data == NULL) return ERR_MEM;
    list->next = (int*)calloc(max_elems * 2, sizeof(int));
    if (list->next == NULL) return ERR_MEM;
    list->prev = list->next + max_elems;

    list->free = 1;
    for (int i = 1; i < max_elems; ++i)
    {
        list->data[i] = list->prev[i] = poison_val;
        list->next[i] = (i + 1) % max_elems;
    }
    list->next[0] = list->prev[0] = 0;

    list->fdump = NULL;

    return ERR_OK;
}

void listDtor(List* list)
{
    free(list->data);
    free(list->next);
    if (list->fdump != NULL) fclose(list->fdump);
}

ErrEnum listVerify(List* list)
{
    if (list       == NULL || list->data == NULL ||
        list->next == NULL || list->prev == NULL)
        return ERR_NULL_LIST;

    int ind = 0;
    for (int i = 0; i < max_elems; ++i)
    {
        if (list->prev[list->next[ind]] != ind) return ERR_INVAL_PREV;
        ind = list->next[ind];
        if (ind == 0) break;
    }
    if (ind != 0) return ERR_LIST_CYCLE;

    ind = list->free;
    for (int i = 0; i < max_elems; ++i)
    {
        ind = list->next[ind];
        if (ind == 0) break;
    }
    if (ind != 0) return ERR_FREE_CYCLE;

    return ERR_OK;
}

ErrEnum listGetFront(List* list, ListElem** elem)
{
    myAssert(list != NULL && elem != NULL);

    if (list->next[0] == 0)
    {
        *elem = NULL;
        return ERR_EMPTY_LIST;
    }
    *elem = list->data + list->next[0];
    return ERR_OK;
}

ErrEnum listGetBack(List* list, ListElem** elem)
{
    myAssert(list != NULL && elem != NULL);

    if (list->prev[0] == 0)
    {
        *elem = NULL;
        return ERR_EMPTY_LIST;
    }
    *elem = list->data + list->prev[0];
    return ERR_OK;
}

ErrEnum listNext(List* list, ListElem* elem, ListElem** ans)
{
    myAssert(list != NULL && elem != NULL && ans != NULL);

    int i = elem - list->data;
    if (i <= 0 || i >= max_elems) return ERR_INVAL_LIST_ELEM;

    if (list->data[i] == poison_val) return ERR_FREE_LIST_ELEM;
    if (list->next[i] == 0)
    {
        *ans = NULL;
        return ERR_LIST_BOUND;
    }
    *ans = list->data + list->next[i];
    return ERR_OK;
}
ErrEnum listPrev(List* list, ListElem* elem, ListElem** ans)
{
    myAssert(list != NULL && elem != NULL && ans != NULL);

    int i = elem - list->data;
    if (i <= 0 || i >= max_elems) return ERR_INVAL_LIST_ELEM;

    if (list->data[i] == poison_val) return ERR_FREE_LIST_ELEM;
    if (list->prev[i] == 0)
    {
        *ans = NULL;
        return ERR_LIST_BOUND;
    }
    *ans = list->data + list->prev[i];
    return ERR_OK;
}

ErrEnum listPushFront(List* list, ListElem val)
{
    myAssert(list != NULL);
    return listInsertBefore(list, list->data + list->next[0], val);
}

ErrEnum listPushBack(List* list, ListElem val)
{
    myAssert(list != NULL);
    return listInsertAfter(list, list->data + list->prev[0], val);
}

ErrEnum listPopFront(List* list, ListElem* val)
{
    myAssert(list != NULL);

    if (list->next[0] == 0) return ERR_EMPTY_LIST;
    if (val != NULL) *val = list->data[list->next[0]];
    return listErase(list, list->data + list->next[0]);
}
ErrEnum listPopBack(List* list, ListElem* val)
{
    if (list == NULL) return ERR_NULL_LIST;

    if (list->prev[0] == 0) return ERR_EMPTY_LIST;
    if (val != NULL) *val = list->data[list->prev[0]];
    return listErase(list, list->data + list->prev[0]);
}

ErrEnum listInsertBefore(List* list, ListElem* elem, ListElem val)
{
    myAssert(list != NULL && elem != NULL);
    if (list->free == 0) return ERR_LIST_OVERFLOW;

    int elem_ind = elem - list->data;
    if (elem_ind < 0 || elem_ind >= max_elems) return ERR_INVAL_LIST_ELEM;
    if (list->data[elem_ind] == poison_val) return ERR_FREE_LIST_ELEM;
    int new_free = list->next[list->free];

    list->data[list->free] = val;
    list->next[list->free] = elem_ind;
    list->next[list->prev[elem_ind]] = list->free;
    list->prev[list->free] = list->prev[elem_ind];
    list->prev[elem_ind] = list->free;
    list->free = new_free;

    return ERR_OK;
}

ErrEnum listInsertAfter(List* list, ListElem* elem, ListElem val)
{
    myAssert(list != NULL && elem != NULL);
    if (list->free == 0) return ERR_LIST_OVERFLOW;

    int elem_ind = elem - list->data;
    if (elem_ind < 0 || elem_ind >= max_elems) return ERR_INVAL_LIST_ELEM;
    if (list->data[elem_ind] == poison_val) return ERR_FREE_LIST_ELEM;
    int new_free = list->next[list->free];

    list->data[list->free] = val;
    list->prev[list->free] = elem_ind;
    list->prev[list->next[elem_ind]] = list->free;
    list->next[list->free] = list->next[elem_ind];
    list->next[elem_ind] = list->free;
    list->free = new_free;

    return ERR_OK;
}

ErrEnum listErase(List* list, ListElem* elem)
{
    myAssert(list != NULL && elem != NULL);
    
    int elem_ind = elem - list->data;
    if (elem_ind <= 0 || elem_ind >= max_elems) return ERR_INVAL_LIST_ELEM;
    if (list->data[elem_ind] == poison_val) return ERR_FREE_LIST_ELEM;

    list->data[elem_ind] = poison_val;
    list->next[list->prev[elem_ind]] = list->next[elem_ind];
    list->prev[list->next[elem_ind]] = list->prev[elem_ind];
    list->next[elem_ind] = list->free;
    list->prev[elem_ind] = poison_val;
    list->free = elem_ind;

    return ERR_OK;
}

ErrEnum listByVal(List* list, ListElem val, ListElem** ans)
{
    myAssert(list != NULL && ans != NULL);

    for (int i = list->next[0]; i != 0; i = list->next[i])
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
    myAssert(list != NULL && ans != NULL);
    if (ind < 0 || ind >= max_elems) return ERR_LIST_INDEX_BOUND; // ind >= max_elems - 1

    int i = 0;
    for (int cnt = 0; cnt <= ind; ++cnt)
    {
        i = list->next[i];
        if (i == 0)
        {
            *ans = NULL;
            return ERR_LIST_INDEX_BOUND;
        }
    }
    *ans = list->data + i;
    return ERR_OK;
}

ErrEnum listMakeGraph(List* list)
{
    myAssert(list != NULL);

    static int first_call = 1;
    char buf[buffer_size] = "";
    if (first_call)
    {
        first_call = 0;
        sprintf(buf, "type NUL > %s/dot-src/dot-src.txt", log_path);
        system(buf);
    }

    sprintf(buf, "%s/dot-src/dot-src.txt", log_path);
    FILE *fout = fopen(buf, "w");
    if (fout == NULL) return ERR_OPEN_FILE;

    fputs("digraph List\n{\nrankdir=LR\n", fout);
    fprintf(fout, "free [shape=Mrecord,label=\"free|<next>%d\"]\n\n", list->free);

    for (int i = 0; i < max_elems; ++i)
        fprintf(fout, "elem%d [shape=Mrecord,label=\"elem%d|<val>%d|<next>%d|%d\"]\n", 
        i, i, list->data[i], list->next[i], list->prev[i]);
    
    for (int i = 0; i < max_elems - 1; ++i)
        fprintf(fout, "elem%d->elem%d[weight=1000,color=\"#00000000\"]\n", i, i + 1);

    int i = 0;
    do
    {
        fprintf(fout, "elem%d:<next>->elem%d:<next>\n", i, list->next[i]);
        i = list->next[i];
    } while (i != 0);

    if (list->free != 0)
    {
        fprintf(fout, "free->elem%d:<next>\n", list->free);
        i = list->free;
        while (list->next[i] != 0)
        {
            fprintf(fout, "elem%d:<next>->elem%d:<next>\n", i, list->next[i]);
            i = list->next[i];
        }
    }

    fputs("}\n", fout);

    fclose(fout);


    sprintf(buf, "dot %s/dot-src/dot-src.txt -Tpng -o%s/dot-img/dot-img-%d.png", 
    log_path, log_path, dump_cnt);
    system(buf);

    return ERR_OK;
}

ErrEnum listDump(List* list)
{
    myAssert(list != NULL);

    static int first_call = 1;
    char buf[buffer_size] = "";
    if (first_call)
    {
        first_call = 0;
        sprintf(buf, "type NUL > %s/dump/dump.html", log_path);
        system(buf);
        sprintf(buf, "%s/dump/dump.html", log_path);
        list->fdump = fopen(buf, "w");
        if (list->fdump == NULL) return ERR_OPEN_FILE;
    }

    FILE *fout = list->fdump;

    fprintf(fout, "<pre>\n\nList [0x%p]\n\nfree: %d\n", list, list->free);

    fprintf(fout, "\ndata [0x%p]: ", list->data);
    for (int i = 0; i < max_elems; ++i)
        fprintf(fout, "%d ", list->data[i]);
    fprintf(fout, "\nnext [0x%p]: ", list->next);
    for (int i = 0; i < max_elems; ++i)
        fprintf(fout, "%d ", list->next[i]);
    fprintf(fout, "\nprev [0x%p]: ", list->prev);
    for (int i = 0; i < max_elems; ++i)
        fprintf(fout, "%d ", list->prev[i]);

    sprintf(buf, "../dot-img/dot-img-%d.png", dump_cnt);
    fprintf(fout, "\n\n<img src=\"%s\" alt=\"graph image\"/>\n\n", buf);


    listMakeGraph(list);
    ++dump_cnt;
    return ERR_OK;
}