#include <stdio.h>

#include <list.h>
#include <error.h>

// temp
#include <stdlib.h>
#include <string.h>

int main(int argc, const char* argv[])
{
    List list = {};
    handleErr(listCtor(&list));

    for (int i = 0; i < 5; ++i)
        handleErr(listPushBack(&list, (i + 1) * 10));

    handleErr(listDump(&list));

    for (int i = 6; i < 10; ++i)
        handleErr(listPushBack(&list, (i + 1) * 10));

    handleErr(listDump(&list));

    listDtor(&list);
    return 0;
}