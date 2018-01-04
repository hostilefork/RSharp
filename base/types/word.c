#include "rsharp.h"

struct type_methods word_methods = {
    NULL,
    MoldWord,
    FormWord,
    NULL,            /* Add */
    NULL,            /* Free */
    NULL,            /* Subtract */
    NULL,            /* Multiply */
    NULL,            /* Divide */
    NULL,            /* Pick */
    CompareWord,
    NULL,            /* EvalPath */
    NULL                /* SetEvalPath */
};

/* --- Words --- */

VALUE NewWord(char *start, int len, UBYTE kind)
{
    VWORD p;

    p = (VWORD) AllocValue();
    p->type = kind;
    p->save = 0;
    p->reserved = 0;
    p->symbol = AddSymbol(start, len);
    p->index = 0;
    p->ctx = global_ctx;
    p->ctx_idx = -1;
    return((VALUE) p);
}

VALUE AltNewWord(VWORD model)
{
    VWORD p;

    p = (VWORD) AllocValue();
    memcpy(p, model, max_value_size);
    p->type = TYPE_WORD;
    return ((VALUE) p);
}

VALUE GetValueWord(VWORD word)
{
    VALUE value;

    value = GetContextDef(word->ctx, word->ctx_idx, word->symbol);
    if (!value)
        /*_ASSERT(0);*/
        return (VALUE) unset_value;
    else
        return value;
}

char CompareWord(VALUE p, VALUE value)
{
    if (IS_ANY_WORD(value))
        if (TYPE(value) == TYPE_REFINEMENT)
            return CompareSymbols(WORD(p)->symbol, REFINEMENT(value)->word->symbol);
        else
            return CompareSymbols(WORD(p)->symbol, WORD(value)->symbol);
    else
        return IS_SERIES(value) ? 1 : -1;                /* scalar! < word! < series! */
}

char *MoldWord(VALUE p, UBYTE *save)
{
    char *s;

    _ASSERT(IS_ANY_WORD(p));

    s = AllocMem(strlen(GetSymbol(WORD(p)->symbol)) + 2);
    switch (TYPE(p))
    {
        case TYPE_GET_WORD:
            s[0] = ':';
            s[1] = '\0';
            return strcat(s, GetSymbol(WORD(p)->symbol));
        case TYPE_SET_WORD:
            s[0] = '\0';
            strcat (s, GetSymbol(WORD(p)->symbol));
            return strcat(s, ":");
        case TYPE_LIT_WORD:
            s[0] = '\'';
            s[1] = '\0';
            return strcat(s, GetSymbol(WORD(p)->symbol));
        default:
            s[0] = '\0';
            return strcat (s, GetSymbol(WORD(p)->symbol));
    }
}

char *FormWord(VALUE p, UBYTE *save)
{
    _ASSERT(IS_ANY_WORD(p));
    *save = 1;
    return GetSymbol(WORD(p)->symbol);
}
