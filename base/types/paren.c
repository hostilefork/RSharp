#include "rsharp.h"


struct type_methods paren_methods = {
    NULL,
    MoldParen,
    MoldParen,        /* Form */
    NULL,            /* Add */
    FreeParen,
    NULL,            /* Subtract */
    NULL,            /* Multiply */
    NULL,            /* Divide */
    PickParen,
    CompareBlock,
    PathEvalBlock,
    SetPathBlock
};


VALUE NewParen(int size)
{
    VBLOCK p;

    p = NewBlock(size);
    p->type = TYPE_PAREN;
    return (VALUE) p;
}

void FreeParen(VALUE p)
{
    FreeBlock(BLOCK(p));
}

int AppendParen(VPAREN p, VALUE val)
{
    return AppendBlock(BLOCK(p), val);
}

VALUE PickParen(VALUE p, long idx)
{
    return PickBlock(p,idx);
}

char *MoldParen(VALUE p, UBYTE *save)
{
    char *s;
    
    _ASSERT(TYPE(p) == TYPE_PAREN);

    s = MoldBlock(p, save);
    s[0] = '(';
    s[strlen(s) - 1] = ')';
    return s;
}

