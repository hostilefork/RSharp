#include "rsharp.h"


VALUE RS_Head(VALUE *args, VALUE *refs)
{
    _ASSERT(IS_SERIES(args[0]));

    SERIES(args[0])->head = 0;
    return args[0];
}

VALUE RS_Headq(VALUE *args, VALUE *refs)
{
    _ASSERT(IS_SERIES(args[0]));

    return SERIES(args[0])->head == 0 ? true_value : false_value;
}

VALUE RS_Tail(VALUE *args, VALUE *refs)
{
    _ASSERT(IS_SERIES(args[0]));

    SERIES(args[0])->head = SERIES(args[0])->tail;
    return args[0];
}

VALUE RS_Tailq(VALUE *args, VALUE *refs)
{
    _ASSERT(IS_SERIES(args[0]));

    return SERIES(args[0])->head == SERIES(args[0])->tail ? true_value : false_value;
}

VALUE RS_Next(VALUE *args, VALUE *refs)
{
    _ASSERT(IS_SERIES(args[0]));

    if (SERIES(args[0])->head < SERIES(args[0])->tail) SERIES(args[0])->head++;
    return args[0];
}

VALUE RS_Back(VALUE *args, VALUE *refs)
{
    _ASSERT(IS_SERIES(args[0]));

    if (SERIES(args[0])->head > 0) SERIES(args[0])->head--;
    return args[0];
}

VALUE RS_Skip(VALUE *args, VALUE *refs)
{
    VSERIES list;
    long steps;

    _ASSERT(IS_SERIES(args[0]));

    list = SERIES(args[0]);
    steps = ((VINTEGER)args[1])->data;
    if (steps >= 0)
    {
        if (list->head < list->tail)
            list->head = MIN(list->head + steps, list->tail);
    }
    else
    {
        if (list->head > 0)
            list->head = MAX((long)list->head + steps, 0);
    }
    return args[0];
}

VALUE RS_At(VALUE *args, VALUE *refs)
{
    VSERIES list;
    long steps;

    _ASSERT(IS_SERIES(args[0]));

    list = SERIES(args[0]);
    steps = ((VINTEGER)args[1])->data;
    if (steps >= 0)
    {
        if (list->head < list->tail)
            list->head = MIN(list->head + (steps ? steps - 1 : 0), list->tail);
    }
    else
    {
        if (list->head > 0)
            list->head = MAX((long)list->head + steps, 0);
    }
    return args[0];
}

VALUE RS_Lengthq(VALUE *args, VALUE *refs)
{
    _ASSERT(IS_SERIES(args[0]));

    return (VALUE)AltNewInteger((long)SERIES(args[0])->tail - SERIES(args[0])->head);
}

VALUE RS_Indexq(VALUE *args, VALUE *refs)
{
    _ASSERT(IS_SERIES(args[0]));

    return (VALUE)AltNewInteger(1L + SERIES(args[0])->head);
}

VALUE RS_Insert(VALUE *args, VALUE *refs)
{
    VALUE value;

    _ASSERT(IS_SERIES(args[0]));

    if (IS_ANY_STRING(args[0]))
        value = InsertString(args, refs);
    else                /* TYPE_ANY_BLOCK */
        value = InsertBlock(args, refs);

    return value;
}

VALUE RS_Pick(VALUE *args, VALUE *refs)
{
    long idx;

    if (VTABLE(args[0]))
    {
        idx = (TYPE(args[1]) == TYPE_LOGIC) ? (LOGIC(args[1])->data ? 0 : 1) : INTEGER(args[1])->data;
        if (!idx)
            return none_value;
        else
            return VTABLE(args[0])->Pick(args[0], idx);
    }
    else
    {
        _ASSERT(0);        /* should not happen */
        return NULL;
    }
}

VALUE RS_Poke(VALUE *args, VALUE *refs)
{
    VALUE value;

    _ASSERT(IS_SERIES(args[0]));

    if (IS_ANY_STRING(args[0]))
        value = PokeString(args[0], args[1], args[2]);
    else                /* TYPE_ANY_BLOCK */
        value = PokeBlock(args[0], args[1], args[2]);

    return value;
}

VALUE RS_Copy(VALUE *args, VALUE *refs)
{
    VALUE value;

    _ASSERT(IS_SERIES(args[0]));

    if (IS_ANY_STRING(args[0]))
        value = CopyString(args, refs);
    else                /* TYPE_ANY_BLOCK */
        value = CopyBlock(args, refs);

    return value;
}
