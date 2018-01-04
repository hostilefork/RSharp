#include "rsharp.h"

struct type_methods op_methods = {
    NULL,                                    /* Dynamically filled to workaround compiler limitation */
    &MoldOp,
    &FormOp,
    NULL,                /* Add */
    NULL,                /* Free */
    NULL,                /* Subtract */
    NULL,                /* Multiply */
    NULL,                /* Divide */
    NULL,                /* Pick */
    CompareOp,
    NULL,                /* EvalPath */
    NULL                /* SetEvalPath */
};

#define OP_LESSER            1
#define OP_GREATER            2
#define OP_LESSER_OR_EQUAL    3
#define OP_GREATER_OR_EQUAL 4

VALUE NewOp(VANYFUNC fun)
{
    VOP p;

    p = (VOP) AllocValue();
    p->type = TYPE_OP;
    p->save = 0;
    p->reserved = 0;
    p->argcount = 2;
    p->specs = fun->specs;
    p->locals = fun->locals;
    p->args = DuplicateArgsArray(fun->args, fun->argcount);
    p->refines = fun->refines;
    p->body = (FUNCPTR)ACTION(fun)->body;

    return (VALUE)p;
}

VALUE MakeOp(VALUE *args, VALUE *refs)
{
    return (VALUE) NewOp(args[0]);
}

void BuildNativeMakeOp(void)
{
    VNATIVE p;

    p = NATIVE(NewNative(BLOCK(Tokenize("operator [any-function!]", NULL)), TYPE_NATIVE));
    p->body = MakeOp;
    op_methods.NativeMakeType = p;
}

char *MoldOp(VALUE p, UBYTE *save)
{
    *save = 1;
    return "op";
}

char *FormOp(VALUE p, UBYTE *save)
{
    *save = 1;
    return "?op?";
}

char CompareOp(VALUE p, VALUE value)
{
    if (TYPE(value) == TYPE_OP)
        return p == value ? 0 : (p < value ? 1 : -1);
    else    
        return 1;                                /* op! < all others */
}

VALUE RS_Add(VALUE *args, VALUE *refs)
{
    return VTABLE(args[0])->Add(args);
}

VALUE RS_Subtract(VALUE *args, VALUE *refs)
{
    return VTABLE(args[0])->Subtract(args);
}

VALUE RS_Multiply(VALUE *args, VALUE *refs)
{
    return VTABLE(args[0])->Multiply(args);
}

VALUE RS_Divide(VALUE *args, VALUE *refs)
{
    return VTABLE(args[0])->Divide(args);
}

VALUE RS_Equalq(VALUE *args, VALUE *refs)
{
    if (IS_SERIES(args[0]) && (TYPE(args[0]) != TYPE(args[1])))
        return false_value;
    else
        return VTABLE(args[0])->Compare(args[0], args[1]) == 0 ? true_value : false_value;
}

VALUE RS_NotEqualq(VALUE *args, VALUE *refs)
{
    return RS_Equalq(args, refs) == true_value ? false_value : true_value;
}

VALUE RS_StrictEqualq(VALUE *args, VALUE *refs)
{
    if (IS_ANY_STRING(args[0]) && IS_ANY_STRING(args[0]))
        return CompareStringCase(args[0], args[1]) == 0 ? true_value : false_value;
    else
    {
        if (RS_Equalq(args, refs) == true_value)
            return TYPE(args[0]) == TYPE(args[1]) ? true_value : false_value;
        else
            return false_value;
    }
}

VALUE RS_Sameq(VALUE *args, VALUE *refs)
{
    if (IS_SERIES(args[0]))
        return args[0] == args[1] ? true_value : false_value;
    else
        return RS_StrictEqualq(args, refs);
}

VALUE DoCompare(VALUE a, VALUE b, char op)
{
    char cmp;

    cmp = VTABLE(a)->Compare(a, b);
    switch (op)
    {
        case OP_LESSER:                cmp = (cmp == 1);  break;
        case OP_GREATER:            cmp = (cmp == -1); break;
        case OP_LESSER_OR_EQUAL:    cmp = (cmp >= 0);  break;
        case OP_GREATER_OR_EQUAL:    cmp = (cmp <= 0);  break;
    }
    return cmp ? true_value : false_value;
}

VALUE CompareValues(VALUE a, VALUE b, char op)
{
    if (IS_SERIES(a) || IS_SERIES(b))
        if (TYPE(a) == TYPE(b))
            return DoCompare(a, b, op);
        else
            return NewError(1, 23, NULL);
    else
        if ((IS_NUMBER(a) || TYPE(a) == TYPE_CHAR) && (IS_NUMBER(b) || TYPE(b) == TYPE_CHAR))
            return DoCompare(a, b, op);
        else
            return NewError(1, 23, NULL);
}

VALUE RS_Lesserq(VALUE *args, VALUE *refs)
{
    return CompareValues(args[0], args[1], OP_LESSER); 
}

VALUE RS_Greaterq(VALUE *args, VALUE *refs)
{
    return CompareValues(args[0], args[1], OP_GREATER); 
}

VALUE RS_LesserOrEqualq(VALUE *args, VALUE *refs)
{
    return CompareValues(args[0], args[1], OP_LESSER_OR_EQUAL); 
}

VALUE RS_GreaterOrEqualq(VALUE *args, VALUE *refs)
{
    return CompareValues(args[0], args[1], OP_GREATER_OR_EQUAL); 
}
