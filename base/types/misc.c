#include "rsharp.h"


struct type_methods none_methods = {
    NULL,
    MoldNone,
    MoldNone,        /* Form */
    NULL,            /* Add */
    NULL,            /* Free */
    NULL,            /* Subtract */
    NULL,            /* Multiply */
    NULL,            /* Divide */
    NULL,            /* Pick */
    CompareNone,
    NULL,                /* EvalPath */
    NULL                /* SetEvalPath */
};

struct type_methods logic_methods = {
    NULL,
    MoldLogic,
    MoldLogic,        /* Form */
    NULL,            /* Add */
    NULL,            /* Free */
    NULL,            /* Subtract */
    NULL,            /* Multiply */
    NULL,            /* Divide */
    NULL,            /* Pick */
    CompareLogic,
    NULL                /* EvalPath */
};

struct type_methods unset_methods = {
    NULL,
    MoldUnset,
    MoldUnset,
    NULL,            /* Add */
    NULL,            /* Free */
    NULL,            /* Subtract */
    NULL,            /* Multiply */
    NULL,            /* Divide */
    NULL,            /* Pick */
    CompareUnset,
    NULL                /* EvalPath */
};

char CompareNone(VALUE p, VALUE value)
{
    return TYPE(value) == TYPE_NONE ? 0 : 1;            /* none! < not none! */
}

char CompareLogic(VALUE p, VALUE value)
{
    if (TYPE(value) == TYPE_LOGIC)
        return p == value ? 0 : (p == false_value ? 1 : -1);
    else
        if (TYPE(value) == TYPE_NONE)
            return p == false_value ? 1 : -1;            /* false < none < true */
        else
            return TYPE(value) == TYPE_UNSET ? -1 : 1;    /* unset! < logic! < not none! */
}

char CompareUnset(VALUE p, VALUE value)
{
    return TYPE(value) == TYPE_UNSET ? 0 : 1;            /* unset! < all others */
}

char *MoldNone(VALUE p, UBYTE *save)
{
    _ASSERT(TYPE(p) == TYPE_NONE);
    *save = 1;
    return "none";
}

char *MoldLogic(VALUE p, UBYTE *save)
{
    _ASSERT(TYPE(p) == TYPE_LOGIC);
    *save = 1;
    return ((VLOGIC)p)->data ? "false" : "true";
}

char *MoldUnset(VALUE p, UBYTE *save)
{
    _ASSERT(TYPE(p) == TYPE_UNSET);
    *save = 1;
    return "unset";
}

VALUE RS_Not(VALUE *args, VALUE *refs)
{
    if ((args[0] == none_value) || (args[0] == false_value))
        return true_value;
    else
        return false_value;
}

VALUE RS_Quit(VALUE *args, VALUE *refs)
{
    FreeAllMem();
    exit(0);
}

/* Case-insensitive comparaison of 2 strings */

UBYTE strcmplower(char *a, char *b)             /* returns: 1 => equal, 0 => different */
{
    while (*a && *b)
        if (!(tolower(*a++) == tolower(*b++)))
            return 0;

    if (*a || *b) 
        return 0;

    return 1;
}
