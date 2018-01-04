#include "rsharp.h"

struct type_methods function_methods = {
    NULL,                                    /* Dynamically filled to workaround compiler limitation */
    MoldFunction,
    FormFunction,
    NULL,                /* Add */
    FreeFunction,
    NULL,                /* Subtract */
    NULL,                /* Multiply */
    NULL,                /* Divide */
    NULL,                /* Pick */
    CompareFunction,
    NULL,                /* EvalPath */
    NULL                /* SetEvalPath */
};

VALUE NewFunction(VBLOCK specs, VBLOCK body)
{
    VFUNCTION p;
    VERROR error;

    p = (VFUNCTION) AllocValue();
    p->type = TYPE_FUNCTION;
    p->save = 0;
    p->reserved = 0;
    p->specs = specs;
    p->locals = NULL;
    p->argcount = 0;
    p->args = NULL;
    p->refines = NULL;
    if (error = ProcessFuncSpecs((VALUE)p))
        return (VALUE) error;
    p->body = body;

    _ASSERT(body);
    BindToContext(p->ctx, p->body);

    return (VALUE) p;
}

void FreeFunction(VALUE p)
{
    UWORD i;
    VFUNCTION func;

    func = FUNCTION(p);
    if (func->argcount > 0)
    {
        for(i = 0; i < func->argcount; i++)
            FreeMem(func->args[i]);
        FreeMem(func->args);
    }
/*    if (func->refines)
    {
        ref = func->refines;
        while (ref)
        {
            if (ref->argcount > 0)
            {
                for(i = 0; i < ref->argcount; i++)
                    FreeMem(ref->args[i]);
                FreeMem(ref->args);
            }
            next = ref->next;
            FreeMem(ref);
            ref = next;
        }
    }
*/
/*    if (func->locals)
        FreeBlock(func->locals);

    FreeBlock(func->specs);
    FreeBlock(func->body);
    FreeContext(func->ctx);
*/
}

char CompareFunction(VALUE p, VALUE value)
{
    char cmp;
    VBLOCK blkA, blkB;

    if (TYPE(p) == TYPE_FUNCTION)
    {
        if (p == value)
            return 0;

        cmp = CompareBlock(FUNCTION(p)->body, FUNCTION(value)->body);
        if (cmp)
            return cmp;
        blkA = FUNCTION(p)->locals;
        blkB = FUNCTION(value)->locals;
        if (!blkA && !blkB)
            return 0;
        if (blkA && blkB)
            return CompareBlock(FUNCTION(p)->locals, FUNCTION(value)->locals);
        return !blkA ? 1 : -1;
    }
    else
        return 1;                        /* function! < all others */    
}

VALUE MakeFunction(VALUE *args, VALUE *refs)
{
    return (VALUE) NewFunction(args[0], args[1]);
}

void BuildNativeMakeFunction(void)
{
    VNATIVE p;

    p = NATIVE(NewNative(BLOCK(Tokenize("spec [block!] body [block!]", NULL)), TYPE_NATIVE));
    p->body = MakeFunction;
    function_methods.NativeMakeType = p;
}

char *MoldFunction(VALUE p, UBYTE *save)
{
    char *res, *s;
    ULONG size;

    size = 64;
    res = AllocMem(size);
    res[0] = '\0';
    strcat(res, "func ");

    s = MoldBlock(FUNCTION(p)->specs, save);
    if (strlen(s) + strlen(res) + 1 > size)
    {
        size += strlen(s) + 1;
        res = ReAllocMem(res, size);
    }
    strcat(res, s);

    s = MoldBlock(FUNCTION(p)->body, save);
    if (strlen(s) + strlen(res) + 1 > size)
        res = ReAllocMem(res, strlen(s) + strlen(res) + 1);
    strcat(res, s);

    return res;
}


char *FormFunction(VALUE p, UBYTE *save)
{
    *save = 1;
    return "?function?";
}

