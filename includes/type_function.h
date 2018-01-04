#ifndef _FUNCTION_H
#define _FUNCTION_H


struct VALUE_FUNCTION {
    HEADER32
    VBLOCK        specs;            /* Function definition block */
    VBLOCK        locals;            /* List of words to define locally (including refinements) */
    UWORD        argcount;        /* Minimum argument number */
    UWORD        totalcnt;        /* Total argument number accounting for refinements */
    UBYTE        **args;            /* Minimum arguments list */
    VREFINEMENT    refines;        /* Linked list of refinements with their argument list */
    VBLOCK        body;            /* Function body block to execute */
    VCONTEXT    ctx;            /* Local function context */
};

typedef struct VALUE_FUNCTION *VFUNCTION;


VALUE    NewFunction(VBLOCK, VBLOCK);
void    FreeFunction(VALUE);
char    CompareFunction(VALUE, VALUE);
void    BuildNativeMakeFunction(void);
char    *MoldFunction(VALUE p, UBYTE *);
char    *FormFunction(VALUE p, UBYTE *);

#endif
