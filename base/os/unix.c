#include "rsharp.h"

#ifdef OS_UNIX

/* Case-insensitive comparaison of two strings */

char strcmpi(char *a, char *b)
{
    ULONG sizeA, sizeB;
    ULONG i = 0;

    sizeA = strlen(a);
    sizeB = strlen(b);
    if (sizeA != sizeB) return sizeA < sizeB ? 1 : -1;
    else
    for(i = 0; i < sizeA; i++, a++, b++)
        if (tolower(*a) != tolower(*b))                        /* case insensitive comparison */
            return *a < *b ? 1 : -1;
    return 0;
}

#endif

