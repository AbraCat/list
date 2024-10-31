#include <stdio.h>

#include <list.h>
#include <error.h>

int main(int argc, const char* argv[])
{
    List list = {};
    listCtor(&list);
    listDump(stdout, &list);
    listDtor(&list);
    return 0;
}