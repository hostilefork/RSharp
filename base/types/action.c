#include "rsharp.h"

struct type_methods action_methods = {
    NULL,
    MoldAction,
    FormAction,
    NULL,                /* Add */
    NULL,                /* Free */
    NULL,                /* Subtract */
    NULL,                /* Multiply */
    NULL,                /* Divide */
    NULL,                /* Pick */
    CompareAction,
    NULL,                /* EvalPath */
    NULL                /* SetEvalPath */
};

VALUE NewAction(VBLOCK specs)
{
    VACTION p;
    VERROR error;

    p = (VACTION) AllocValue();
    p->type = TYPE_ACTION;
    p->save = 0;
    p->reserved = 0;
    p->specs = specs;
    p->locals = NULL;
    p->argcount = 0;
    p->args = NULL;
    p->refines = NULL;
    p->body = NULL;
    if (error = ProcessFuncSpecs((VALUE)p))
        return (VALUE) error;

    return (VALUE)p;
}

char *MoldAction(VALUE p, UBYTE *save)
{
    *save = 1;
    return "action";
}


char *FormAction(VALUE p, UBYTE *save)
{
    *save = 1;
    return "?action?";
}

char CompareAction(VALUE p, VALUE value)        /* ? char => int ? */
{
    if (TYPE(value) == TYPE_ACTION)
        return (char)(p == value ? 0 : (p < value ? 1 : -1));
    else    
        return 1;                                /* action! < all others */
}

void NewExec(VTASK p, VBLOCK blk, ACTIONPTR callback)
{
    p->exec += 1;
    p->exec->ident = ACTION_DO;
    p->exec->result = unset_value;
    p->exec->on_expr_done = callback;
    p->exec->locals_idx = (UWORD)(p->top + 1);
    p->exec->base = p->call;
    p->exec->blk = blk;
    p->exec->idx = blk->head;
    p->value = unset_value;
}

VALUE RS_Do(VTASK p, VALUE *args, VALUE *refs)
{
    VBLOCK blk;
    VSTRING str;
    VALUE val;

    blk = NULL;

    if (IS_ANY_BLOCK(args[0]))
        blk = BLOCK(args[0]);

    if (TYPE(args[0]) == TYPE_STRING)
    {
        str = STRING(args[0]);
        val = Tokenize(str->data + str->head, str->data + str->tail);
        if (TYPE(val) == TYPE_ERROR)
        {
            p->value = val;
            return GO_POP_CALL;
        }
        blk = BLOCK(val);
    }

    if (!blk)
    {
        p->value = args[0];
        return GO_POP_CALL;
    }

    NewExec(p, blk, NULL);
    return NULL;
}

VALUE RS_Try(VTASK p, VALUE *args, VALUE *refs)
{
    RS_Do(p, args, NULL);
    p->exec->ident = ACTION_TRY;
    return NULL;
}

VALUE RS_Catch(VTASK p, VALUE *args, VALUE *refs)
{
    RS_Do(p, args, NULL);
    p->exec->ident = ACTION_CATCH;
    if (refs)
        PUSH(args[1]);
    return NULL;
}

VALUE RS_Throw(VTASK p, VALUE *args, VALUE *refs)
{
    VALUE name;
    VBLOCK names;
    ULONG i;
    bool match;

    name = (refs ? args[1] : NULL);
    match = false;
    do
    {
        if (ThrowException(p, ACTION_CATCH, 31) == GO_POP_CALL)
            return GO_POP_CALL;

        if (name)
        {
            names = BLOCK(p->values[p->exec->locals_idx]);
            if (TYPE(names) == TYPE_BLOCK)
            {
                for (i = names->head; i < names->tail; i++)
                    if ((TYPE(names->data[i]) == TYPE_WORD) &&
                        (WORD(names->data[i])->symbol == WORD(name)->symbol))
                    {
                        match = true;
                        break;
                    }
            }
            else
                match = (WORD(names)->symbol == WORD(name)->symbol);

            if (!match)
                p->exec -= 1;        /* This 'catch doesn't match, search next one */
        }
    }
    while (name && !match);
    p->value = args[0];
    return GO_POP_DO;
}

VALUE RS_Reduce(VTASK p, VALUE *args, VALUE *refs)
{
    if (TYPE(args[0]) != TYPE_BLOCK)
    {
        p->value = args[0];
        return GO_POP_CALL;
    }

    NewExec(p, BLOCK(args[0]), &ReduceNext);
    PUSH(NewBlock(0));                            /* locals */
    return NULL;
}

VALUE ReduceNext(VTASK p, VALUE *args, VALUE *refs)
{
    VBLOCK blk;

    blk = BLOCK(p->values[p->exec->locals_idx]);
    _ASSERT(p->value != NULL);
    AppendBlock(blk, p->value);

    if (p->exec->idx == p->exec->blk->tail)
        p->value = p->values[p->exec->locals_idx];            /* Return the reduced block as last value */

    return NULL;
}

VALUE RS_If(VTASK p, VALUE *args, VALUE *refs)
{
    if ((args[0] == none_value) || (args[0] == false_value))
    {
        p->value = none_value;
        return GO_POP_CALL;
    }

    NewExec(p, BLOCK(args[1]), NULL);
    return NULL;
}

VALUE RS_Either(VTASK p, VALUE *args, VALUE *refs)
{
    if ((args[0] == none_value) || (args[0] == false_value))
        p->value = args[2];
    else
        p->value = args[1];
    
    NewExec(p, BLOCK(p->value), NULL);
    return NULL;
}

VALUE RS_Loop(VTASK p, VALUE *args, VALUE *refs)
{
    long count;

    if ((count = INTEGER(args[0])->data) <= 0)
    {
        p->value = unset_value;
        return GO_POP_CALL;
    }

    NewExec(p, BLOCK(args[1]), &LoopNext);
    p->exec->ident = ACTION_LOOP;
    PUSH((void *)count);                        /* locals */
    return NULL;
}

VALUE LoopNext(VTASK p, VALUE *args, VALUE *refs)
{
    ULONG *len;

    if (p->exec->idx == p->exec->blk->tail)
    {
        len = (ULONG *)&p->values[p->exec->locals_idx];
        if (--(*len) == 0)
        {
            p->value = unset_value;
            return (void *)1;
        }
        else
        {
            p->exec->idx = p->exec->blk->head;
            return NULL;
        }
    }
    return NULL;
}

VALUE RS_Exit(VTASK p, VALUE *args, VALUE *refs)
{
    p->value = unset_value;
    return ThrowException(p, ACTION_FUNC, 27);
}

VALUE RS_Return(VTASK p, VALUE *args, VALUE *refs)
{
    p->value = args[0];
    return ThrowException(p, ACTION_FUNC, 27);
}

VALUE RS_Break(VTASK p, VALUE *args, VALUE *refs)
{
    p->value = (refs ? args[0] : unset_value);
    return ThrowException(p, ACTION_LOOP, 30);
}

VALUE RS_Any(VTASK p, VALUE *args, VALUE *refs)
{
    NewExec(p, BLOCK(args[0]), &AnyNext);
    return NULL;    
}

VALUE AnyNext(VTASK p, VALUE *args, VALUE *refs)
{
    if ((p->value != none_value) && (p->value != false_value))
        p->exec->idx = p->exec->blk->tail;

    return NULL;
}

VALUE RS_All(VTASK p, VALUE *args, VALUE *refs)
{
    NewExec(p, BLOCK(args[0]), &AllNext);
    return NULL;    
}

VALUE AllNext(VTASK p, VALUE *args, VALUE *refs)
{
    if ((p->value == none_value) || (p->value == false_value))
    {
        p->value = none_value;
        p->exec->idx = p->exec->blk->tail;
    }

    return NULL;
}

VALUE RS_Until(VTASK p, VALUE *args, VALUE *refs)
{
    NewExec(p, BLOCK(args[0]), &UntilNext);
    p->exec->ident = ACTION_LOOP;
    return NULL;
}

VALUE UntilNext(VTASK p, VALUE *args, VALUE *refs)
{
    if (p->exec->idx == p->exec->blk->tail)
    {
        if ((p->value == none_value) || (p->value == false_value))
        {
            p->exec->idx = p->exec->blk->head;
            return NULL;
        }
        else
            return (void *)1;
    }
    return NULL;
}

VALUE RS_While(VTASK p, VALUE *args, VALUE *refs)
{
    NewExec(p, BLOCK(args[0]), &WhileNext);
    p->exec->ident = ACTION_LOOP;
    return NULL;
}

VALUE WhileNext(VTASK p, VALUE *args, VALUE *refs)
{
    if (p->exec->idx == p->exec->blk->tail)
    {
        if (p->exec->blk == BLOCK(args[0]))
        {
            if ((p->value != none_value) && (p->value != false_value))
            {
                p->exec->blk = BLOCK(args[1]);
                p->exec->idx = BLOCK(args[1])->head;    
            }
        }
        else
        {
            p->exec->blk = BLOCK(args[0]);
            p->exec->idx = BLOCK(args[0])->head;
        }
    }
    return NULL;
}
