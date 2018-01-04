#include "rsharp.h"


struct type_methods refinement_methods = {
    NULL,
    MoldRefinement,
    FormRefinement,
    NULL,                /* Add */
    FreeRefinement,
    NULL,                /* Subtract */
    NULL,                /* Multiply */
    NULL,                /* Divide */
    NULL,                /* Pick */
    CompareRefinement,
    NULL,                /* EvalPath */
    NULL                /* SetEvalPath */
};

VALUE NewRefinement(char *start, int len)
{
    VREFINEMENT p;

    p = (VREFINEMENT) AllocValue();
    p->type = TYPE_REFINEMENT;
    p->save = 0;
    p->reserved = 0;
    p->word = NewWord(start, len, TYPE_WORD);
    p->argcount = 0;
    p->next = NULL;
    p->args = NULL;
    return p;
}

void FreeRefinement(VALUE p)
{
    VREFINEMENT ref;
    UWORD i;

    ref = REFINEMENT(p);

    if (ref->argcount > 0)
    {
        for(i = 0; i < ref->argcount; i++)
            FreeMem(ref->args[i]);
        FreeMem(ref->args);
    }
}

char CompareRefinement(VALUE ref, VALUE value)
{
    return CompareWord(REFINEMENT(ref)->word, value);
}

char *MoldRefinement(VALUE p, UBYTE *save)
{
    char *s;

    s = AllocMem(strlen(GetSymbol(REFINEMENT(p)->word->symbol)) + 2);
    s[0] = '/';
    s[1] = '\0';
    return strcat(s, MoldWord(REFINEMENT(p)->word, save));    /* Molded string should be freed ?!? */
}

char *FormRefinement(VALUE p, UBYTE *save)
{
    return MoldWord(REFINEMENT(p)->word, save);                /* Molded string should be freed ?!? */
}
