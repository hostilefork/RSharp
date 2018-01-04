#ifndef _OP_H
#define _OP_H

struct VALUE_OP {
    HEADER32
    VBLOCK        specs;            /* Function definition block */
    VBLOCK        locals;            /* List of words to define locally (including refinements) */
    UWORD        argcount;        /* Minimum argument number */
    UWORD        totalcnt;        /* Total argument number accounting for refinements */
    UBYTE        **args;            /* Arguments type bitset list */
    VREFINEMENT    refines;        /* Optionnal refines with their argument list */
    FUNCPTR        body;            /* Body code to execute */
};

typedef struct VALUE_OP *VOP;

void    BuildNativeMakeOp(void);
char    *MoldOp(VALUE, UBYTE *);
char    *FormOp(VALUE, UBYTE *);
char    CompareOp(VALUE, VALUE);

VALUE    RS_Add(VALUE *, VALUE *);
VALUE    RS_Subtract(VALUE *, VALUE *);
VALUE    RS_Multiply(VALUE *, VALUE *);
VALUE    RS_Divide(VALUE *, VALUE *);
VALUE    RS_Equalq(VALUE *, VALUE *);
VALUE    RS_NotEqualq(VALUE *, VALUE *);
VALUE    RS_StrictEqualq(VALUE *, VALUE *);
VALUE    RS_Sameq(VALUE *, VALUE *);
VALUE    RS_Lesserq(VALUE *, VALUE *);
VALUE    RS_Greaterq(VALUE *, VALUE *);
VALUE    RS_LesserOrEqualq(VALUE *, VALUE *);
VALUE    RS_GreaterOrEqualq(VALUE *, VALUE *);

#endif
