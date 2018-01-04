#include "rsharp.h"
#include "natives.h"

struct type_methods native_methods = {
    NULL,
    MoldNative,
    FormNative,
    NULL,            /* Add */
    NULL,            /* Free */
    NULL,            /* Subtract */
    NULL,            /* Multiply */
    NULL,            /* Divide */
    NULL,            /* Pick */
    NULL,            /* Compare */
    NULL,            /* EvalPath */
    NULL            /* SetEvalPath */
};

VALUE NewNative(VBLOCK specs, UBYTE kind)
{
    VNATIVE p;
    VERROR error;

    p = (VNATIVE) AllocValue();
    p->type = kind;
    p->save = 0;
    p->reserved = 0;
    p->specs = specs;
    p->locals = NULL;
    p->argcount = 0;
    p->args = NULL;
    p->refines = NULL;
    if (error = ProcessFuncSpecs((VALUE)p))
        return (VALUE) error;

    return (VALUE)p;
}

void MakeNative(VCONTEXT ctx, struct native_def *def, UBYTE kind)
{
    VBLOCK spec_blk;
    VNATIVE p;

    spec_blk = BLOCK(Tokenize(def->specs, NULL));

    _ASSERT(HEADER(spec_blk)->type == TYPE_BLOCK);  /* Expecting a block! */
    _ASSERT(BLOCK(spec_blk)->tail > 0);                /* and non empty ! */

    p = NATIVE(NewNative(spec_blk, kind));
    _ASSERT(TYPE(p) != TYPE_ERROR);
    p->body = def->body;
    SetContextDef(ctx, NewWord(def->name, strlen(def->name), TYPE_WORD), p);
}

void MakeAllNatives(VCONTEXT ctx)
{
    ULONG i;

    /* native! */
    for(i = 0; i < ((sizeof(natives) / sizeof(struct native_def))); i++)
        MakeNative(ctx, &natives[i], TYPE_NATIVE);

    /* action! */
    for(i = 0; i < ((sizeof(actions) / sizeof(struct action_def ))); i++)
        MakeNative(ctx, (struct native_def *)&actions[i], TYPE_ACTION);
}

char CompareNative(VALUE p, VALUE value)
{
    if (TYPE(value) == TYPE_NATIVE)
        return p == value ? 0 : (p < value ? 1 : -1);
    else    
        return 1;                                /* native! < all others */
}

char *MoldNative(VALUE p, UBYTE *save)
{
    *save = 1;
    return "native";
}


char *FormNative(VALUE p, UBYTE *save)
{
    *save = 1;
    return "?native?";
}


