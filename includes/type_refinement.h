#ifndef _REFINEMENT_H
#define _REFINEMENT_H

#include "type_context.h"

typedef struct VALUE_REFINEMENT *VREFINEMENT;

struct VALUE_REFINEMENT {
    HEADER32    
    VWORD        word;        /* Word value */
    UWORD        argcount;    /* Argument number */
    UWORD        index;        /* Refinement order index */
    VREFINEMENT next;
    char        **args;        /* Argument list */        
};


VALUE    NewRefinement(char *, int);
void    FreeRefinement(VALUE);
char    CompareRefinement(VALUE, VALUE);
char    *MoldRefinement(VALUE, UBYTE *);
char    *FormRefinement(VALUE, UBYTE *);

#endif

