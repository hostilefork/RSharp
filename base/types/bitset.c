#include "rsharp.h"


/* === Type definition === */


struct VALUE_BITSET {
    UBYTE    type;
    UWORD    refcount;
    char    data[32];
};

typedef struct VALUE_BITSET *VBITSET;


/* === API === */


VALUE new_Bitset(void)
{
    VBITSET p;
    p = (VBITSET) AllocValue();
    p->type = TYPE_BITSET;
    p->refcount = 0;
    /*p->data = 0;*/
    return((VALUE) p);
}

void free_Bitset(VBITSET p)
{
    FreeMem((void *)p);
}


