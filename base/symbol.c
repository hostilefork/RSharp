#include "rsharp.h"


/* Case-insensitive comparaison of a string and a array of n bytes*/

char strncmpi(char *a, char *b, ULONG n)
{
    ULONG i;

    for(i = 0; i < n; i++, a++, b++)
        if (tolower(*a) != tolower(*b))                        /* case insensitive comparison */
            return *a < *b ? 1 : -1;
    return 0;
}

UBYTE strncmp_equal(char *a, char *b, int n)        /* returns: 1 => equal, 0 => different */
{
    while (*a && n--)                            /* should add && *b to avoid memory violation in protected OS */
        if (!(tolower(*a++) == tolower(*b++)))
            return 0;

    if (*a || n)
        return 0;

    return 1;
}

char CompareSymbols(ULONG a, ULONG b)
{
    ULONG sizeA, sizeB;

    sizeA = strlen(symbols.list[a]);
    sizeB = strlen(symbols.list[b]);
    if (sizeA == sizeB)
        return strncmpi(symbols.list[a], symbols.list[b], sizeA);
    else
        return sizeA < sizeB ? 1 : -1;
}

long FindSymbol(char *start, int len)
{
    long i;

    for (i = 0; i < (long)symbols.tail; i++)
        if (strncmp_equal(symbols.list[i], start, len))
            return i;
    return -1;
}

ULONG AddSymbol(char *start, int len)
{
    char *str;
    long i;

    for (i = 0; i < (long)symbols.tail; i++)
        if ((strncmp(symbols.list[i], start, len) == 0) && (strlen(symbols.list[i]) == (UWORD)len))
            return (ULONG)i;

    if (symbols.tail == symbols.slots)
    {
        symbols.slots *= 2;
        symbols.list = realloc(symbols.list, symbols.slots * sizeof(char *));
    }

    _ASSERT(len < 1024);                    /* Temporary limit word size to 1024 */
    str = AllocMemForever(len + 1);            /* Extra byte allocated for ending zero */
    strncpy(str, start, len);
    str[len] = '\0';
    symbols.list[symbols.tail++] = str;
    return (ULONG)symbols.tail - 1;
}

char *GetSymbol(ULONG idx)
{
    return symbols.list[idx];
}

void InitSymbolsTable(void)
{
    symbols.slots = 1000;
    symbols.list = malloc(symbols.slots  * sizeof(char *));
    symbols.tail = 0;

/*    sym_local = AddSymbol("local", 5);    */        /* Useful for function spec block processing */
}

void FreeSymbolsTable(void)
{
    free(symbols.list);
}


void DumpSymbols()
{
    ULONG i;

    for (i = 0; i < symbols.tail; i++)
        printf("%s ", symbols.list[i]);
}
