#include "rsharp.h"

struct type_methods integer_methods = {
    NULL,
    MoldInteger,
    MoldInteger,        /* Form */
    AddInteger,
    NULL,                /* Free */
    SubtractInteger,
    MultiplyInteger,
    DivideInteger,
    NULL,                /* Pick */
    CompareInteger,
    NULL,                /* EvalPath */
    NULL                /* SetEvalPath */
};

VALUE NewInteger(char *start,  int len)
{
    VINTEGER p;
    int i;

    p = (VINTEGER) AllocValue();
    p->type = TYPE_INTEGER;
    p->save = 0;
    p->reserved = 0;
    i = 10 + ((start[0] == '+') || (start[0] == '-') ? 1 : 0);
    if (len > i)
        return NewError(0, 1, NULL);

    i = sscanf(start, "%ld", &p->data);
    if (!i 
      || ((p->data < 0) && (start[0] != '-')) 
      || ((p->data > 0) && (start[0] == '-')))
        return NewError(0, 1, NULL);

    return((VALUE) p);
}

VALUE AltNewInteger(long value)
{
    VINTEGER p;

    p = (VINTEGER) AllocValue();
    p->type = TYPE_INTEGER;
    p->save = 0;
    p->reserved = 0;
    p->data = value;
    return((VALUE) p);
}

char CompareInteger(VALUE i, VALUE value)
{
    long data;

    switch (TYPE(value))
    {
        case TYPE_INTEGER:    data = INTEGER(value)->data;     break;
        case TYPE_CHAR:        data = (long) CHAR(value)->data; break;
        default: return 1;
    }
    if (INTEGER(i)->data == data)
        return 0;

    return (INTEGER(i)->data < data) ? 1 : -1;
}

char *MoldInteger(VALUE p, UBYTE *save)
{
    char *s;

    _ASSERT(TYPE(p) == TYPE_INTEGER);

    s = (char *) AllocMem(64);
    sprintf(s, "%ld", INTEGER(p)->data);
    return s;
}

VALUE Calc(long a, long b, char op)
{
    long res, signA, signB;

    signA = a&(1<<31);
    signB = b&(1<<31);
    switch(op)
    {
        case INT_ADD: 
            res = a + b; 
            if ((signA == signB) && (signA != (res&(1<<31)))) op = -1;
            break;
        case INT_SUB: 
            res = a - b;
            if ((signA != signB) && (signA != (res&(1<<31)))) op = -1;
            break;
        case INT_MUL:
            res = a * b; 
            /* TBD: Write an overflow test!! */
            break;
        case INT_DIV: 
            if (b == 0)
                return (VALUE) NewError(2, 20, NULL);
            res = a / b; 
            break;
    }

    return (VALUE)(op == -1 ? NewError(2, 25, NULL) : AltNewInteger(res));
}

VALUE AddInteger(VALUE *args)
{
    _ASSERT(TYPE(args[1]) == TYPE_INTEGER);
    return Calc(INTEGER(args[0])->data, INTEGER(args[1])->data, INT_ADD);
}

VALUE SubtractInteger(VALUE *args)
{
    _ASSERT(TYPE(args[1]) == TYPE_INTEGER);
    return Calc(INTEGER(args[0])->data, INTEGER(args[1])->data, INT_SUB);
}

VALUE MultiplyInteger(VALUE *args)
{
    _ASSERT(TYPE(args[1]) == TYPE_INTEGER);
    return Calc(INTEGER(args[0])->data, INTEGER(args[1])->data, INT_MUL);
}

VALUE DivideInteger(VALUE *args)
{
    _ASSERT(TYPE(args[1]) == TYPE_INTEGER);
    return Calc(INTEGER(args[0])->data, INTEGER(args[1])->data, INT_DIV);
}

VALUE RS_Zeroq(VALUE *args, VALUE *refs)
{
    char val ;

    switch(TYPE(args[0]))
    {
        case TYPE_INTEGER: val = INTEGER(args[0])->data == 0; break;
/*        case TYPE_DECIMAL: val = DECIMAL(args[0])->data == 0; break;    */
        case TYPE_CHAR: val = CHAR(args[0])->data == 0; break;
    }
    return val ? true_value : false_value;
}
