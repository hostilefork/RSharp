#ifndef _STRINGS_H
#define _STRINGS_H

/* === Type definition === */

struct VALUE_STRING {
    HEADER32
    ULONG    slots;
    ULONG    head;
    ULONG    tail;
    char    *data;
};

typedef struct VALUE_STRING *VSTRING;


VALUE    NewString(char *, int);
void    FreeString(VALUE);
VSTRING    NextString(VSTRING);
VSTRING    BackString(VSTRING);
VALUE    PathEvalString(VALUE, VALUE);
VALUE    SetPathString(VALUE, VALUE, VALUE);
VALUE    InsertString(VALUE *, VALUE *);
VALUE    PickString(VALUE, long);
VALUE    PokeString(VALUE, VALUE, VALUE);
VALUE    CopyString(VALUE *, VALUE *);
char    CompareString(VALUE, VALUE);
char    CompareStringCase(VALUE, VALUE);
char*    MoldString(VALUE, UBYTE *);
char*    FormString(VALUE, UBYTE *);

/* vn */

VALUE    RS_Lowercase(VALUE *, VALUE *);
VALUE    RS_Uppercase(VALUE *, VALUE *);

#endif
