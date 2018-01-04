#include "rsharp.h"

#define SET_BITSET(array, x) array[x >> 3] |= 1 << (x % 8);
#define MAKE_CHARSET(x) (TYPE(x) == TYPE_BLOCK) ? SetTypes(x) : charset_anyvalue

UBYTE charset_anyvalue[32] = {
    0xFE,0xF7,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

void SetVirtualType(UBYTE *cs, UBYTE type)
{
    switch(type)
    {
        case TYPE_ANY_WORD:
            SET_BITSET(cs, TYPE_WORD);
            SET_BITSET(cs, TYPE_LIT_WORD);
            SET_BITSET(cs, TYPE_SET_WORD);
            SET_BITSET(cs, TYPE_GET_WORD);
            SET_BITSET(cs, TYPE_REFINEMENT);
            break;

        case TYPE_ANY_STRING:
            SET_BITSET(cs, TYPE_STRING);
            break;

        case TYPE_ANY_BLOCK:
            SET_BITSET(cs, TYPE_BLOCK);
            SET_BITSET(cs, TYPE_PATH);
            SET_BITSET(cs, TYPE_SET_PATH);
            SET_BITSET(cs, TYPE_LIT_PATH);
            SET_BITSET(cs, TYPE_PAREN);
            break;

        case TYPE_NUMBER:
            SET_BITSET(cs, TYPE_INTEGER);
            SET_BITSET(cs, TYPE_DECIMAL);
            break;

        case TYPE_ANY_FUNC:
            SET_BITSET(cs, TYPE_NATIVE);
            SET_BITSET(cs, TYPE_OP);
            SET_BITSET(cs, TYPE_FUNCTION);
            break;

        case TYPE_SERIES:
            SET_BITSET(cs, TYPE_BLOCK);
            SET_BITSET(cs, TYPE_PATH);
            SET_BITSET(cs, TYPE_SET_PATH);
            SET_BITSET(cs, TYPE_LIT_PATH);
            SET_BITSET(cs, TYPE_PAREN);
            SET_BITSET(cs, TYPE_STRING);
            break;

        case TYPE_ANY:
            memset(cs, 0xFF, 32);
            break;
    }
}

UBYTE *SetTypes(VBLOCK blk)
{
    VALUE p;
    ULONG i;
    UBYTE type;
    UBYTE *cs;

    cs = AllocMem(32);
    memset(cs, 0, 32);

    for (i = blk->head; i < blk->tail; i++)
    {
        p = (VALUE) blk->data[i];
        if (TYPE(p) == TYPE_WORD)
        {
            p = GetValueWord(WORD(p));
            if (TYPE(p) == TYPE_UNSET)
            {
                FreeMem(cs);
                return NULL;
            }
        }
        type = (TYPE(p) == TYPE_DATATYPE) ? DATATYPE(p)->data : TYPE(p);
        if (IS_VIRTUAL_TYPE(type))
            SetVirtualType(cs, type);
        else
            cs[type >> 3] |= 1 << (type % 8);
    }
    return cs;
}

void BuildLocalCtx(VANYFUNC f, VBLOCK locals)
{
    VFUNCTION func;
    ULONG i;

    if (TYPE(f) != TYPE_FUNCTION)
        return;

    func = FUNCTION(f);
    func->ctx = NewContext(0);

    for(i = 0; i < locals->tail; i++)
        if (TYPE(locals->data[i]) == TYPE_REFINEMENT)
            SetContextDef(func->ctx, WORD(REFINEMENT(locals->data[i])->word), NULL);
        else
            SetContextDef(func->ctx, WORD(locals->data[i]), NULL);

    f->locals = locals;
}

VALUE ProcessFuncSpecs(VALUE p)
{
    ULONG i;
    UWORD cnt;
    VANYFUNC func;
    VBLOCK    locals, specs;
    VREFINEMENT refine;

    _ASSERT(IS_ANY_FUNC(p));

    func = ANYFUNC(p);
    specs = func->specs;
    locals = NULL;

    /* Arguments processing */

    for (i = specs->head; i < specs->tail; i++)
    {
        if (TYPE(specs->data[i]) == TYPE_WORD)
        {
            if (!func->args)
            {
                func->args = AllocMem(4 * sizeof(char *));        /* A passer en liste chainée */
                locals = NewBlock(0);
            }

            AppendBlock(locals, specs->data[i]);

            if (i + 1 == specs->tail)
                func->args[func->argcount] = charset_anyvalue;
            else
            {
                func->args[func->argcount] = MAKE_CHARSET(specs->data[i + 1]);
                if (func->args[func->argcount] == NULL)
                    return NewError(1, 11, specs);
            }

            func->argcount++;
        }
        if (TYPE(specs->data[i]) == TYPE_REFINEMENT)
            break;
    }

    if (i == specs->tail)
    {
        if (!locals)
            locals = NewBlock(0);
        BuildLocalCtx(func, locals);
        return NULL;
    }

    /* Refinement processing */

    cnt = 1;
    refine = func->refines = specs->data[i];
    refine->index = cnt++;
    if (!locals)
        locals = NewBlock(0);
    AppendBlock(locals, refine);

    for (i++; i < specs->tail; i++)
    {
        if (TYPE(specs->data[i]) == TYPE_WORD)
        {
            if (!refine->args)
                refine->args = AllocMem(4 * sizeof(char *));        /* A passer en liste chainée */

            AppendBlock(locals, specs->data[i]);

            if (i + 1 == specs->tail)
                refine->args[refine->argcount] = (char *)charset_anyvalue;
            else
            {
                refine->args[refine->argcount] = (char *)(MAKE_CHARSET(specs->data[i + 1]));

                if (refine->args[refine->argcount] == NULL)
                    return NewError(1, 11, specs);
            }

            refine->argcount++;
        }
        if (TYPE(specs->data[i]) == TYPE_REFINEMENT)
        {
            refine->next = REFINEMENT(specs->data[i]);
            refine = refine->next;
            refine->index = cnt++;
        }
    }

    BuildLocalCtx(func, locals);

    return NULL;    /* No errors */
}

void *DuplicateArgsArray(UBYTE **args, UWORD count)
{
    void **p;
    UWORD i;

    p = AllocMem(count * sizeof(char *));
    for(i = 0; i < count; i++)
    {
        p[i] = AllocMem(32);
        memcpy(p[i], args[i], 32);
    }
    return p;
}
