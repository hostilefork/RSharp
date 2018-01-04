#ifndef _CONTEXT_H
#define _CONTEXT_H

#include "type_block.h"

typedef struct VALUE_WORD *VWORD;

/* === Type definition === */

struct ctx_def
{
    ULONG    symbol;            /* index of a symbol string referenced in the symbols table */
    VALUE    value;            /* value referenced by this name or NULL if unset */
    ULONG    index;            /* if type(value) = series!, index value for this definition */
};

struct VALUE_CONTEXT {
    HEADER32
    struct ctx_def *table;    /* Array of pointers on context definition structures */
    ULONG    slots;            /* Number of available places */
    ULONG    tail;            /* Index of last VALUE */
};

typedef struct VALUE_CONTEXT *VCONTEXT;

VALUE    NewContext(int);
void    FreeContext(VALUE);
VALUE    SetContextDef(VCONTEXT, VWORD, VALUE);
VALUE    GetContextDef(VCONTEXT, long, ULONG);
VBLOCK    BindToContext(VCONTEXT, VBLOCK);

VALUE    RS_Valueq(VALUE *, VALUE *);
VALUE    RS_Unset(VALUE *, VALUE *);
VALUE    RS_Get(VALUE *, VALUE *);
VALUE    RS_Set(VALUE *, VALUE *);

#endif
