#ifndef _ANYFUNC_H
#define _ANYFUNC_H

struct VALUE_ANYFUNC {
    HEADER32
    VBLOCK        specs;            /* Function definition block */
    VBLOCK        locals;            /* List of words to define locally (including refinements) */
    UWORD        argcount;        /* Minimum argument number */
    UWORD        totalcnt;        /* Total argument number accounting for refinements */
    UBYTE        **args;            /* Arguments type bitset list */
    VREFINEMENT    refines;        /* Optionnal refines with their argument list */
    void        *body;            /* Body code to execute */
};

typedef struct VALUE_ANYFUNC *VANYFUNC;

VALUE    ProcessFuncSpecs(VALUE);
void    *DuplicateArgsArray(UBYTE **, UWORD);

#endif
