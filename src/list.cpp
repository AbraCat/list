#include <stdlib.h>

#include <list.h>

const char log_path[] = "./log";
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

ErrEnum listGetFront(List* list, ListElem** elem)
{
    if (list == NULL || elem == NULL) return ERR_OK; //
    if (list->next[0] == 0)
    {
        *elem = NULL;
        return ERR_OK; //
    }
    *elem = list->data + list->next[0];
    return ERR_OK;
}

ErrEnum listGetBack(List* list, ListElem** elem)
{
    if (list == NULL || elem == NULL) return ERR_OK; //
    if (list->prev[0] == 0)
    {
        *elem = NULL;
        return ERR_OK; //
    }
    *elem = list->data + list->prev[0];
    return ERR_OK;
}

ErrEnum listNext(List* list, ListElem* elem, ListElem** ans)
{
    if (list == NULL || elem == NULL || ans == 0) return ERR_OK; //

    int i = elem - list->data;
    if (i <= 0 || i >= max_elems) return ERR_OK; //

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
    if (i <= 0 || i >= max_elems) return ERR_OK; //

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

    return listInsertBefore(list, list->data + list->next[0], val);
}

ErrEnum listPushBack(List* list, ListElem val)
{
    // err check

    return listInsertAfter(list, list->data + list->prev[0], val);
}

ErrEnum listPopFront(List* list, ListElem* val)
{
    // err check
    if (list->next[0] == 0) return ERR_OK; //
    if (val != NULL) *val = list->data[list->next[0]];
    return listErase(list, list->data + list->next[0]);
}
ErrEnum listPopBack(List* list, ListElem* val)
{
    // err check
    if (list->prev[0] == 0) return ERR_OK; //
    if (val != NULL) *val = list->data[list->prev[0]];
    return listErase(list, list->data + list->prev[0]);
}

ErrEnum listInsertBefore(List* list, ListElem* elem, ListElem val)
{
    // err check
    if (list->free == 0) return ERR_OK; //

    int elem_ind = elem - list->data;
    if (elem_ind < 0 || elem_ind >= max_elems) return ERR_OK;
    if (list->data[elem_ind] == poison_val) return ERR_OK; //
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
    // err check
    if (list->free == 0) return ERR_OK; //

    int elem_ind = elem - list->data;
    if (elem_ind < 0 || elem_ind >= max_elems) return ERR_OK;
    if (list->data[elem_ind] == poison_val) return ERR_OK; //
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
    // err check
    
    int elem_ind = elem - list->data;
    if (elem_ind <= 0 || elem_ind >= max_elems) return ERR_OK;
    if (list->data[elem_ind] == poison_val) return ERR_OK; //

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
    // err check

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
    // err check (in particular, ind < 0)

    int i = 0;
    for (int cnt = 0; cnt <= ind; ++cnt)
    {
        i = list->next[i];
        if (i == 0)
        {
            *ans = NULL;
            return ERR_OK; // index out of bound
        }
    }
    *ans = list->data + i;
    return ERR_OK;
}

ErrEnum listMakeGraph(List* list)
{
    if (list == NULL) return ERR_OK; //

    char buf[buffer_size] = "";

    sprintf(buf, "type NUL > %s/dot-src/dot-src-%d.txt", log_path, dump_cnt);
    system(buf);
    sprintf(buf, "%s/dot-src/dot-src-%d.txt", log_path, dump_cnt);
    FILE* fout = fopen(buf, "w");
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



    sprintf(buf, "dot %s/dot-src/dot-src-%d.txt -Tpng -o%s/dot-img/dot-img-%d.png", 
    log_path, dump_cnt, log_path, dump_cnt);
    system(buf);

    return ERR_OK;
}

ErrEnum listDump(List* list)
{
    if (list == NULL) return ERR_OK; //

    char buf[buffer_size] = "";

    sprintf(buf, "type NUL > %s/dump/dump-%d.html", log_path, dump_cnt);
    system(buf);
    sprintf(buf, "%s/dump/dump-%d.html", log_path, dump_cnt);
    FILE* fout = fopen(buf, "w");
    if (fout == NULL) return ERR_OPEN_FILE;

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
    fprintf(fout, "\n\n<img src=\"%s\" alt=\"graph image\"/>", buf);

    fclose(fout);



    listMakeGraph(list);
    ++dump_cnt;
    return ERR_OK;
}