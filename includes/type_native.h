#ifndef _NATIVE_H
#define _NATIVE_H

struct VALUE_NATIVE {
    HEADER32
    VBLOCK        specs;            /* Function definition block */
    VBLOCK        locals;            /* List of words to define locally (including refinements) */
    UWORD        argcount;        /* Minimum argument number */
    UWORD        totalcnt;        /* Total argument number accounting for refinements */
    UBYTE        **args;            /* Arguments type bitset list */
    VREFINEMENT    refines;        /* Optionnal refines with their argument list */
    FUNCPTR        body;            /* Native C function to execute */
};

typedef struct VALUE_NATIVE *VNATIVE;

VALUE    NewNative(VBLOCK, UBYTE);
void    MakeAllNatives(VCONTEXT);
void    FreeNative(VNATIVE);
char    CompareNative(VALUE, VALUE);
char    *MoldNative(VALUE, UBYTE *);
char    *FormNative(VALUE, UBYTE *);

#endif
