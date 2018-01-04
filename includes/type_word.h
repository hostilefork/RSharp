#ifndef _WORD_H
#define _WORD_H

#include "type_context.h"

extern ULONG max_value_size;    /* defined in mem.c */


struct VALUE_WORD {
    HEADER32    
    long        symbol;        /* Word string index in global symbols table */
    ULONG        index;        /* If VALUE = serie! => index in the serie */
    VCONTEXT    ctx;        /* Current Context of this word */
    long        ctx_idx;    /* Cached index of symbol entry in ctx */
};

/* typedef struct VALUE_WORD *VWORD; -- defined in context.h */

VALUE    NewWord(char *, int, UBYTE);
VALUE    AltNewWord(VWORD);
void    FreeWord(VALUE);
VALUE    GetValueWord(VWORD);
char    CompareWord(VALUE, VALUE);
char    *MoldWord(VALUE, UBYTE *);
char    *FormWord(VALUE, UBYTE *);

#endif
