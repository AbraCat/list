#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <colors.h>

const int n_errs = 13;
ErrDescr err_arr[] = {{ERR_OK, "No error"}, 

{ERR_NULL_LIST, "NULL list pointer"},
{ERR_INVAL_LIST_ELEM, "Invalid list element pointer"},
{ERR_FREE_LIST_ELEM, "Invalid list element pointer"},
{ERR_LIST_BOUND, "Out of list bounds"},
{ERR_LIST_OVERFLOW, "List overflow"},
{ERR_LIST_INDEX_BOUND, "List index out of bound"},
{ERR_LIST_CYCLE, "Infinite cycle in list"},
{ERR_FREE_CYCLE, "Infinite cycle in list of free elements"},
{ERR_INVAL_PREV, "next and prev contradict"},
{ERR_EMPTY_LIST, "Empty list"},

{ERR_MEM, "Couldn't allocate memory"},
{ERR_OPEN_FILE, "Couldn't open file"}};

void myAssertFn(int expr, const char* str_expr, const char* file, int line, const char* function)
{
    if (expr)
        return;

    printf("%sAssertion failed: %s at %s:%d (%s)%s\n", RED_STR, str_expr, file, line, function, DEFAULT_STR);
    exit(expr);
}

void getErrDescr(ErrEnum num, const char** descr)
{
    for (int i = 0; i < n_errs; ++i)
    {
        if (err_arr[i].num == num)
        {
            *descr = err_arr[i].descr;
            return;
        }
    }
    *descr = NULL;
}

void handleErrFn(ErrEnum num, const char* file, int line, const char* function)
{
    if (num == ERR_OK)
        return;

    const char* descr = NULL;
    getErrDescr(num, &descr);

    if (descr == NULL)
        printf("%sUnknown error (%d) at %s:%d (%s)%s\n", RED_STR, (int)num, file, line, function, DEFAULT_STR);
    else
        printf("%sError: %s at %s:%d (%s)%s\n", RED_STR, descr, file, line, function, DEFAULT_STR);

    exit(num);
}