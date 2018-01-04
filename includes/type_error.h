#ifndef _ERROR_H
#define _ERROR_H

struct VALUE_ERROR {
    HEADER32
    UWORD    cat;        /* Error category */
    UWORD    code;        /* Error number */
    char    *message;    /* Description string */
    VALUE    exec_blk;
};

typedef struct VALUE_ERROR *VERROR;

VALUE    NewError(UWORD, UWORD, VBLOCK);
void    FreeError(VALUE);
char    CompareError(VALUE, VALUE);
char    *MoldError(VALUE, UBYTE *);

VALUE    RS_Errorq(VALUE *, VALUE *);

#endif
