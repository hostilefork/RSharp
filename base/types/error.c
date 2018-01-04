#include "rsharp.h"

struct type_methods error_methods = {
    NULL,                /* MakeNative */
    MoldError,
    MoldError,            /* Form */
    NULL,                /* Add */
    FreeError,            /* Free */
    NULL,                /* Subtract */
    NULL,                /* Multiply */
    NULL,                /* Divide */
    NULL,                /* Pick */
    CompareError,
    NULL,                /* EvalPath */
    NULL                /* SetEvalPath */
};

char *errors[] = {
/*0*/    "Invalid Word!",
        "Invalid Integer!",
        "Invalid Get-word!",
        "Invalid Set-word!",
/*4*/    "Missing \"",
        "Missing ] at end-of-script",
        "Out of range or past end",
        "Word has no value",
/*8*/    "Missing argument",
        "Invalid Path!",
        "Invalid Refinement!",
        "Invalid argument",
/*12*/    "Cannot use path on this value",
        "Function doesn't have this refinement",
        "Invalid argument type",
        "Invalid part count argument",
/*16*/    "Missing [ at start-of-block",
        "Invalid lit-word!",
        "Missing ) at end-of-paren",
        "Missing ( at start-of-paren",
/*20*/    "Division by zero",
        "Invalid Char!",
        "Word needs a value",
        "Type mismatch",
/*24*/    "Invalid /part count",
        "Integer Overflow",
        "Stack Overflow",
        "Return or exit not in function",
/*28*/    "Index out of range",
        "Invalid path value",
        "Nothing to break",
        "No catch"
};

char *cat[] = {
    "Syntax",
    "Script",
    "Math",
    "Throw"
};


/* === API === */


VALUE NewError(UWORD kind, UWORD ern, VBLOCK ctx)
{
    VERROR p;

    p = (VERROR) AllocValue();
    p->type = TYPE_ERROR;
    p->save = 0;
    p->reserved = 0;
    p->message = AllocMem(256);
    p->cat = kind;
    p->code = ern;
    _ASSERT(kind < 4);
    _ASSERT(ern < 32);
    _ASSERT(strlen(errors[ern]) < 256);
    sprintf(p->message, "** %s error: %s", cat[kind], errors[ern]);
    p->exec_blk = ctx;
    return((VALUE) p);
}

void FreeError(VALUE p)
{
    FreeMem(ERROR(p)->message);
}

char CompareError(VALUE p, VALUE value)
{
    int cat, code;

    if (TYPE(value) == TYPE_ERROR)
    {
        cat = ERROR(p)->cat - ERROR(value)->cat;
        code = ERROR(p)->code - ERROR(value)->code;
        if (!cat && !code)
            return 0;
        else
            if (cat)
                return cat < 0 ? 1 : -1;
            else
                return code < 0 ? 1 : -1;
    }
    else    
        return 1;                                /* action! < all others */
}

char *MoldError(VALUE p, UBYTE *save)
{
    *save = 1;
    return "error";
}

VALUE RS_Errorq(VALUE *args, VALUE *refs)
{
    return (TYPE(args[0]) == TYPE_ERROR) ? true_value : false_value;
}
