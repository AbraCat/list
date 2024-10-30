#include <stdio.h>

#include <list.h>
#include <error.h>

int main(int argc, const char* argv[])
{
    List list = {};
    listCtor(&list);

    for (int i = 0; i < 5; ++i)
        listPushBack(&list, (i + 1) * 10);

    listDump(stdout, &list);
    listErase(&list, list.data + 3);
    listDump(stdout, &list);

    listDtor(&list);
    return 0;
}