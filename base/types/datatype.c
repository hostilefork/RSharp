#include "rsharp.h"


struct type_methods datatype_methods = {
    NULL,
    MoldDatatype,
    FormDatatype,
    NULL,                /* Add */
    NULL,                /* Free */
    NULL,                /* Subtract */
    NULL,                /* Multiply */
    NULL,                /* Divide */
    NULL,                /* Pick */
    CompareDatatype,
    NULL,                /* EvalPath */
    NULL                /* SetEvalPath */
};

VALUE NewDatatype(UBYTE kind)
{
    VDATATYPE p;

    p = (VDATATYPE) AllocValue();
    p->type = TYPE_DATATYPE;
    p->save = 0;
    p->reserved = 0;
    p->data = kind;
    return((VALUE) p);
}    

void InitDatatypes(void)
{
    struct type_def *dt;
    char *s;
    UBYTE i;


    datatypes = AllocMemForever(256 * sizeof(struct type_def));
    dt = datatypes;
    memset(dt, 0, 256 * sizeof(struct type_def));
    
    /* Real Types */
    dt[TYPE_UNSET].name            = "unset!";
    dt[TYPE_BLOCK].name            = "block!";
    dt[TYPE_WORD].name            = "word!";
    dt[TYPE_SET_WORD].name        = "set-word!";
    dt[TYPE_GET_WORD].name        = "get-word!";
    dt[TYPE_LIT_WORD].name        = "lit-word!";
    dt[TYPE_INTEGER].name        = "integer!";
    dt[TYPE_DECIMAL].name        = "decimal!";
    dt[TYPE_STRING].name        = "string!";
    dt[TYPE_CHAR].name            = "char!";
    dt[TYPE_BITSET].name        = "bitset!";
    dt[TYPE_ERROR].name            = "error!";
    dt[TYPE_NATIVE].name        = "native!";
    dt[TYPE_CONTEXT].name        = "context!";
    dt[TYPE_NONE].name            = "none!";
    dt[TYPE_LOGIC].name            = "logic!";
    dt[TYPE_DATATYPE].name        = "datatype!";
    dt[TYPE_REFINEMENT].name    = "refinement!";
    dt[TYPE_PATH].name            = "path!";
    dt[TYPE_SET_PATH].name        = "set-path!";
    dt[TYPE_LIT_PATH].name        = "lit-path!";
    dt[TYPE_FUNCTION].name        = "function!";
    dt[TYPE_OP].name            = "op!";
    dt[TYPE_TUPLE].name         = "tuple!";
    dt[TYPE_PORT].name             = "port!";
    dt[TYPE_BINARY].name        = "binary!";
    dt[TYPE_ACTION].name         = "action!";
    dt[TYPE_PAREN].name         = "paren!";

    /* Virtual Types */
    dt[TYPE_ANY_WORD].name        = "any-word!";
    dt[TYPE_ANY_STRING].name    = "any-string!";
    dt[TYPE_ANY_BLOCK].name        = "any-block!";
    dt[TYPE_NUMBER].name        = "number!";
    dt[TYPE_ANY_FUNC].name        = "any-function!";
    dt[TYPE_SERIES].name        = "series!";
    dt[TYPE_ANY].name            = "any-type!";

    for (i = 0;; i++)
    {
        if (s = dt[i].name)
        {
            dt[i].ref = NewDatatype(i);
            SetContextDef(global_ctx, NewWord(s, strlen(s), TYPE_WORD), dt[i].ref);
        }
        if (i == 255) break;
    }
}


void *isDatatypeReady()                    /* for debugging purpose only */
{
    return datatypes;
}

char *GetTypeName(VALUE p)                /* for debugging purpose only */
{
    return datatypes[TYPE(p)].name;
}

char CompareDatatype(VALUE p, VALUE value)
{
    if (TYPE(p) == TYPE_DATATYPE)
        if (DATATYPE(p)->data == DATATYPE(value)->data)
            return 0;
        else
            return strcmpi(datatypes[DATATYPE(p)->data].name,
                           datatypes[DATATYPE(value)->data].name);
    else
        return 1;                        /* datatype! < all others */
}

char *MoldDatatype(VALUE p, UBYTE *save)
{
    *save = 1;
    return datatypes[DATATYPE(p)->data].name;
}

char *FormDatatype(VALUE p, UBYTE *save)
{
    char *s, *ret;

    s = datatypes[DATATYPE(p)->data].name;
    ret = AllocMem(strlen(s) + 1);
    *ret = '\0';
    strcat(ret, s);
    ret[strlen(s) - 1] = '\0';
    return ret;
}

VALUE RS_Typeq(VALUE *args, VALUE *refs)
{
    return (VALUE) datatypes[TYPE(args[0])].ref;
}

VALUE RS_Make(VALUE *args, VALUE *refs)
{
    UBYTE type;

    type = (TYPE(args[0]) == TYPE_DATATYPE) ? DATATYPE(args[0])->data : TYPE(args[0]);
    return methods[type]->NativeMakeType;
}
