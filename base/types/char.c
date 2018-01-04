#include "rsharp.h"

struct type_methods char_methods = {
    NULL,
    MoldChar,
    MoldChar,            /* Form */
    NULL,                /* Add */
    NULL,
    NULL,                /* Subtract */
    NULL,                /* Multiply */
    NULL,                /* Divide */
    NULL,                /* Pick */
    CompareChar,
    NULL,                /* EvalPath */
    NULL                /* SetEvalPath */
};

VALUE NewChar(char c, char escaped)
{
    VCHAR p;

    p = (VCHAR) AllocValue();
    p->type = TYPE_CHAR;
    p->save = 0;
    p->reserved = 0;
    if (escaped == 1)
    {
        switch(c)
        {
            case '-': p->data = '\t'; break;
            case '/': p->data = '\n'; break;
            default:
                if ((c >= 64) && (c <= 95))
                    p->data = c - 64;
                else
                    p->data = c;
        }
    }
    else
        p->data = (UBYTE)(escaped == -1 ? 0 : c);

    return((VALUE) p);
}

char CompareChar(VALUE c, VALUE value)
{
    long data;

    switch (TYPE(value))
    {
        case TYPE_INTEGER:    data = INTEGER(value)->data;     break;
        case TYPE_CHAR:        data = (long) CHAR(value)->data; break;
        default: return 1;
    }
    if (CHAR(c)->data == data)
        return 0;

    return (CHAR(c)->data < data) ? 1 : -1;
}

char *MoldChar(VALUE p, UBYTE *save)
{
    char *s, c;

    c = CHAR(p)->data;

    s = AllocMem(6);
    s[0] = '#';
    s[1] = '"';

    if (c < 32)
    {
        s[2] = '^';
        switch(c)
        {
            case 9:  s[3] = '-'; break;
            case 10: s[3] = '/'; break;
            default: s[3] = 64 + c;
        }
        s[4] = '"';
        s[5] = 0;
    }
    else
    {
        s[2] = c;
        s[3] = '"';
        s[4] = 0;
    }
    return s;
}
