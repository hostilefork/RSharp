#include "rsharp.h"

struct type_methods context_methods = {
    NULL,
    NULL,                /* Mold */
    NULL,                /* Form */
    NULL,                /* Add */
    FreeContext,
    NULL,                /* Subtract */
    NULL,                /* Multiply */
    NULL,                /* Divide */
    NULL,                /* Pick */
    NULL,                /* Compare */
    NULL,                /* EvalPath */
    NULL                /* SetEvalPath */
};


VALUE NewContext(int size)
{
    VCONTEXT p;

    p = (VCONTEXT) AllocValue();
    p->type = TYPE_CONTEXT;
    p->save = 0;
    p->reserved = 0;
    size = ROUND16(size);  /* size increased to the next closest multiple of 16 */
    p->table = AllocMem(size * (sizeof(struct ctx_def)));
    memset(p->table, 0, size * (sizeof(struct ctx_def)));
    p->slots = size;
    p->tail = 0;
    return((VALUE) p);
}

void FreeContext(VALUE p)
{
    FreeMem(CONTEXT(p)->table);
}

void ExpandContext(VCONTEXT ctx, int size)
{
    _ASSERT(0); /* Not implemented */
}

long FindDef(VCONTEXT ctx, ULONG sym)
{
    ULONG i;

    for(i = 0; i < ctx->tail; i++)
        if (ctx->table[i].symbol == sym || strcmplower(GetSymbol(ctx->table[i].symbol), GetSymbol(sym)))
            return (long)i;

    return -1;
}

VALUE SetContextDef(VCONTEXT ctx, VWORD word, VALUE value)
{
    struct ctx_def *def;
    long pos;

    if (word->ctx != ctx)
    {
        word->ctx = ctx;
        word->ctx_idx = -1;
    }

    pos = word->ctx_idx;
    if (pos != -1 || ((pos = FindDef(ctx, word->symbol)) != -1))    /* test if word has been defined in this ctx */
    {
        _ASSERT((ULONG)pos < ctx->tail);
        def = &ctx->table[pos];
        /* def->symbol = word->symbol; */        /* optimization for last symbol "case" version used */
        def->value = value;
        if (value && IS_SERIES(value))
        {
            word->index = SERIES(value)->head;
            def->index = word->index;
        }
    }
    else
    {
        _ASSERT(ctx->tail <= ctx->slots);

        if (ctx->tail == ctx->slots)
            ExpandContext(ctx, 0);

        ctx->table[ctx->tail].symbol = word->symbol;
        ctx->table[ctx->tail].value = value;
        if (value && IS_SERIES(value))
        {
            word->index = SERIES(value)->head;
            ctx->table[ctx->tail].index = word->index;
        }
        word->ctx = ctx;
        word->ctx_idx = ctx->tail;
        ctx->tail++;
    }
    return value;
}

VALUE GetContextDef(VCONTEXT ctx, long idx, ULONG sym)
{
    struct ctx_def *def;
    long pos;

    if (idx == -1)
    {
        pos = FindDef(ctx, sym);
        if (pos == -1)
            return NULL;
        else
        {
            return ctx->table[pos].value;        /* gestion de def->value->index à revoir !! */
        }
    }
    else
    {
        _ASSERT((ULONG)idx < ctx->tail);
        def = &ctx->table[idx];
        if (def->value && IS_SERIES(def->value))
        {
            /*if (SERIES(def->value)->head != def->index)
              {
                serie = CloneValue(def->value);
                SERIES(serie)->head = def->index;
                return serie;
              } */
        }
        return def->value;
    }
}

VBLOCK BindToContext(VCONTEXT ctx, VBLOCK blk)
{
    VWORD word;
    ULONG i;
    long pos;

    for(i = 0; i < blk->tail; i++)
    {
        if (IS_ANY_WORD(blk->data[i]))
        {
            if (TYPE(blk->data[i]) == TYPE_REFINEMENT)
                word = REFINEMENT(blk->data[i])->word;
            else
                word = WORD(blk->data[i]);
            pos = FindDef(ctx, word->symbol);
            if (pos != -1)
            {
                word->ctx = ctx;
                word->ctx_idx = pos;
            }
        }
        if (TYPE(blk->data[i]) == TYPE_BLOCK)
            BindToContext(ctx, blk->data[i]);        /* Recursively bind all words in sub-blocks */
    }
    return blk;
}

VALUE RS_Valueq(VALUE *args, VALUE *refs)
{
    VWORD w;

    if (TYPE(args[0]) == TYPE_WORD)
    {
        w = WORD(args[0]);
        return (GetContextDef(w->ctx, w->ctx_idx, w->symbol)) ? true_value : false_value;
    }
    else
        return true_value;
}

VALUE RS_Unset(VALUE *args, VALUE *refs)
{
    if (TYPE(args[0]) == TYPE_BLOCK)
    {
        VBLOCK blk;
        ULONG i;

        blk = BLOCK(args[0]);
        for (i = blk->head; i < blk->tail; i++)
            if (IS_ANY_WORD(blk->data[i]))
                SetContextDef(WORD(blk->data[i])->ctx, WORD(blk->data[i]), NULL);
    }
    else
        SetContextDef(WORD(args[0])->ctx, WORD(args[0]), NULL);

    return unset_value;
}

VALUE RS_Get(VALUE *args, VALUE *refs)
{
    VALUE result;

    result = GetValueWord(args[0]);
    if (!refs && TYPE(result) == TYPE_UNSET)
        return NewError(1, 7, NULL);
    else
        return result;
}

VALUE RS_Set(VALUE *args, VALUE *refs)
{
    VBLOCK blk;
    VWORD word;
    VALUE *value;
    ULONG limit, i;


    if (!refs && TYPE(args[1]) == TYPE_UNSET)
        return NewError(1, 22, NULL);

    if (TYPE(args[0]) == TYPE_BLOCK)
    {
        blk = BLOCK(args[0]);
        if (TYPE(args[1]) == TYPE_BLOCK)
        {
            limit = BLOCK(args[1])->tail - BLOCK(args[1])->head;
            value = (limit == 0) ? (VALUE *)&none_value : BLOCK(args[1])->data;
        }
        else
        {
            limit = (ULONG)-1;
            value = args + 1;
        }
        for(i = blk->head; i < blk->tail; i++)
        {
            word = WORD(blk->data[i]);
            if (IS_ANY_WORD(word))
                SetContextDef(word->ctx, word, *value);
            else
                return NewError(1, 11, NULL);
            
            if (limit != (ULONG)-1)
            {
                if (i + 1 < limit)
                    value++;
                else
                    value = (VALUE) &none_value;
            }
        }
    }
    else
        SetContextDef(WORD(args[0])->ctx, WORD(args[0]), args[1]);
        
    return args[1];
}
