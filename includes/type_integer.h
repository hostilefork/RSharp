#ifndef _INTEGER_H
#define _INTEGER_H

#define INT_ADD        1
#define INT_SUB        2
#define INT_MUL        3
#define INT_DIV        4

/* === Type definition === */

struct VALUE_INTEGER {
    HEADER32
    long    data;
};

typedef struct VALUE_INTEGER *VINTEGER;


VALUE    NewInteger(char *, int);
VALUE    AltNewInteger(long);
void    FreeInteger(VALUE);
char    *MoldInteger(VALUE, UBYTE *);
VALUE    AddInteger(VALUE *);
VALUE    SubtractInteger(VALUE *);
VALUE    MultiplyInteger(VALUE *);
VALUE    DivideInteger(VALUE *);
char    CompareInteger(VALUE, VALUE);

VALUE    RS_Zeroq(VALUE *, VALUE *);

#endif
