#include "rsharp.h"

VTASK *task_list[32];
ULONG task_count;

ACTIONPTR    OnExprDone;

VTASK NewTask()
{
    VTASK p;

    p = (VTASK) malloc(sizeof(struct VALUE_TASK));
    if (p == NULL)
        printf("***Out of memory !\n");

    p->funcs = malloc(FUNC_STK_SIZE * sizeof(struct exec_slot));
    if (p->funcs == NULL)
        printf("***Out of memory !\n");

    p->calls = malloc(FUNC_STK_SIZE * sizeof(struct call_slot));
    if (p->calls == NULL)
        printf("***Out of memory !\n");

    p->values = malloc(1024 * sizeof(void *));
    if (p->funcs == NULL)
        printf("***Out of memory !\n");

    p->last = (CALLSLOT *)((char *)p->funcs + (FUNC_STK_SIZE - 1) * sizeof(struct call_slot));

    return p;
}

void InitInterpreter()
{
    /* init code */
}

void InitTask(VTASK task, VBLOCK code)
{
    task->top = 0;
    task->infix = 0;
    task->last = NULL;
    task->word = NULL;
    task->path = NULL;
    task->value = NULL;

    task->call = task->calls;
    task->exec = task->funcs;
    task->exec->ident = ACTION_DO;
    task->exec->on_expr_done = NULL;
    task->exec->blk = code;
    task->exec->idx = code->head;
    task->exec->base = task->call;
    task->exec->result = unset_value;

    task->values[0] = NULL;
}

void FreeTask(VTASK p)
{
    free(p->calls);
    free(p->values);
    free(p->funcs);
    free(p);
}

UBYTE CheckRefinements(VTASK p)
{
    ULONG i;
    VANYFUNC func;
    VREFINEMENT ref, funcref;

    func = p->call->func;
    func->totalcnt = func->argcount;

    if (!func->refines)
        return 0; 

    funcref = NULL; 
    p->call->refs_idx = p->top + 1;

    for (i = p->path->head + 1; i < p->path->tail; i++)
    {
        if (TYPE(p->path->data[i]) != TYPE_WORD)
            return 0;

        ref = func->refines; 
        while (ref)
        {
            /* Should be optimized */
            if (strcmplower(GetSymbol(WORD(p->path->data[i])->symbol), GetSymbol(ref->word->symbol)))
            {
                PUSH(ref);                /* Push refinement on value stack */
                func->totalcnt += ref->argcount;
                if (funcref)
                    funcref->next = ref;
                funcref = ref;
                break;
            }
            ref = ref->next;
        }
    }
    p->call->args_idx = p->top + 1;        /* save refinements list ending pos */
    return 1;
}

UBYTE TypeCheckArgs(VTASK p)
{
    VANYFUNC func;
    VFUNCTION fun;
    VREFINEMENT ref;
    VBLOCK blk;
    UWORD i, j, cnt, ri;
    ULONG l; 
    UBYTE local_ctx;
    VALUE *stack;

    func = p->call->func;

    /* Init all locals to none */
    if (local_ctx = (TYPE(func) == TYPE_FUNCTION))
    {
        fun = FUNCTION(func);
        blk = fun->locals;
        for(l = blk->head; l < blk->tail; l++)
            if (TYPE(blk->data[l]) == TYPE_REFINEMENT)
                SetContextDef(fun->ctx, REFINEMENT(blk->data[l])->word, none_value);
            else
                SetContextDef(fun->ctx, WORD(blk->data[l]), none_value);
    }

    stack = p->values + p->call->args_idx;

    /* Mandatory arguments type checking */

    for (i = 0; i < func->argcount ; i++)
    {
        if (!(IN_BITSET(func->args[i], TYPE(stack[i]))))
            return 0;                                            /* Type check failed */
        if (local_ctx)
            SetContextDef(fun->ctx, fun->locals->data[i], stack[i]);
    }

    if (!func->refines || !p->call->refs_idx) 
        return 1;

    /* Optional arguments type checking */

    ri = i;                                                        /* refinement arg index in fun->locals */
    for (j = p->call->refs_idx; j < p->call->args_idx; j++)
    {
        ref = REFINEMENT(p->values[j]);
        if (local_ctx)
            SetContextDef(fun->ctx, ref->word , true_value);
        ri++;                                                    /* skip the refinement */

        for (cnt = 0; cnt < ref->argcount; cnt++)
        {
            if (!(IN_BITSET(ref->args[cnt], TYPE(stack[i]))))
                return 0;                                        /* Type check failed */
            if (local_ctx)
                SetContextDef(fun->ctx, fun->locals->data[ri], stack[i]);
            i++;
            ri++;                                                /* next argument word in locals */
        }
        p->values[j] = (void *)(ULONG)ref->index;                        /* Exchange in stack ref pointer <=> ref index */
    }
    return 1;
}

void *PathEval(VTASK p, VPATH path, UBYTE set)
{
    VALUE    refine, reduced;
    ULONG    pos;

    pos = path->head;
    reduced = path->data[pos++];

    while (pos < path->tail)
    {
        refine = path->data[pos++];

        if (IS_ANY_WORD(reduced))
            reduced = GetValueWord(WORD(reduced));

        if (TYPE(reduced) == TYPE_UNSET)
        {
            p->value = NewError(1, 7, NULL);
            return GO_ERROR;
        }
        if (set && pos == path->tail)
        {
            if (VTABLE(reduced)->SetPath)
            {
                p->value = VTABLE(reduced)->SetPath(reduced, refine, p->value);
                return GO_POP_CALL;
            }
            else
            {
                p->value = NewError(1, 12, NULL);
                return GO_ERROR;
            }
        }
        if (IS_ANY_FUNC(reduced))
        {
            p->value = reduced;
            return GO_PUSH_CALL;
        }
        if (VTABLE(reduced)->PathEval)
            reduced = VTABLE(reduced)->PathEval(reduced, refine);
        else
        {
            p->value = NewError(1, 12, NULL);
            return GO_ERROR;
        }
    }
    p->value = reduced;
    return GO_PUSH_VALUE;
}

VALUE ThrowException(VTASK p, ULONG kind, UWORD ern)
{
    while (p->exec->ident != kind)
    {
        if (p->exec == p->funcs)
        {
            p->value = NewError(3, ern, NULL);
            return GO_POP_CALL;
        }
        p->exec -= 1;
    }
    return GO_POP_DO;
}

void EvalStateMachine(VTASK p)
{
    VALUE    ret;


ESM_FETCH_VALUE:

    if (p->exec->idx == p->exec->blk->tail)
        if (p->exec == p->funcs)
            goto ESM_FINISH;
        else
            goto ESM_POP_DO;

    p->value = (VALUE) p->exec->blk->data[p->exec->idx++]; 
    switch (TYPE(p->value))
    {
        case TYPE_PAREN:    goto ESM_EXEC_DO;
        case TYPE_SET_PATH:
        case TYPE_SET_WORD: goto ESM_PUSH_CALL;
        case TYPE_GET_WORD:
        case TYPE_WORD:        goto ESM_EVAL_WORD;
        case TYPE_PATH:        goto ESM_EVAL_PATH;
        case TYPE_LIT_WORD:
            p->value = AltNewWord(p->value);
            goto ESM_PUSH_VALUE; 

        case TYPE_LIT_PATH:
            p->value = AltNewPath(p->value);
            goto ESM_PUSH_VALUE; 

        default: goto ESM_PUSH_VALUE;
    }


ESM_EVAL_WORD:

    p->word = WORD(p->value);
    p->value = GetContextDef(p->word->ctx, p->word->ctx_idx, p->word->symbol);
    if (p->value == NULL)
    {
        p->value = NewError(1, 7, p->exec->blk);
        goto ESM_RAISE_ERROR;
    }
    if ((TYPE(p->word) != TYPE_GET_WORD) && IS_ANY_FUNC(p->value))
    {
        p->path = NULL;                    /* No refinement */
        ANYFUNC(p->value)->totalcnt = ANYFUNC(p->value)->argcount;
        goto ESM_PUSH_CALL;
    }
    goto ESM_PUSH_VALUE;


ESM_EVAL_PATH:

    p->path = PATH(p->value);
    ret = PathEval(p, p->path, 0);
    switch ((ULONG)ret)
    {
        case (ULONG)GO_PUSH_VALUE:    goto ESM_PUSH_VALUE;
        case (ULONG)GO_PUSH_CALL:    goto ESM_PUSH_CALL;
        case (ULONG)GO_ERROR:        goto ESM_RAISE_ERROR;
    }


ESM_PUSH_CALL:
            
    if (p->call == p->last)
    {
        p->value = NewError(1, 26, p->exec->blk);        /* Stack Overflow */
        goto ESM_RAISE_ERROR;
    }
        
    p->call += 1;
    p->call->func = p->value;
    p->call->on_expr_done = NULL;
    p->call->referer = p->path ? (VALUE)p->path : (VALUE)p->word;
    p->call->args_idx = p->top + (p->infix ? 0 : 1);
    p->call->refs_idx = 0;

    if ((TYPE(p->value) == TYPE_SET_WORD) || (TYPE(p->value) == TYPE_SET_PATH))
    {
        p->call->remaining = 1;
        goto ESM_FETCH_VALUE;
    }

    if (p->path)
    {
        if (!CheckRefinements(p))
        {
            p->value = NewError(1, 13, p->exec->blk);
            goto ESM_RAISE_ERROR;
        }
    }
    else
        p->call->refs_idx = 0;

    p->call->remaining = ANYFUNC(p->value)->totalcnt;        /* Argument number needed for the pending call */
    if (p->call->remaining == 0)
        goto ESM_EXEC_CALL;

    if (p->infix)
    {
        p->infix = 0;
        p->call->remaining--;                /* 1st argument already on stack */
    }
    goto ESM_FETCH_VALUE;


ESM_PUSH_VALUE:

    /* Infix operator priority detection (should be optimized in speed) */
    p->infix = (
        (p->exec->idx < p->exec->blk->tail - 1) 
        && (TYPE(p->exec->blk->data[p->exec->idx]) == TYPE_WORD)
        && (TYPE(GetValueWord(WORD(p->exec->blk->data[p->exec->idx]))) == TYPE_OP)
    );

    if (p->call == p->exec->base)
    {                                        /* No pending call */ 
        if (TYPE(p->value) == TYPE_ERROR)
            goto ESM_RAISE_ERROR;

        if (!p->infix)
            goto ESM_EXPR_DONE;
    }

    PUSH(p->value);                            /* Push a new argument */

    if (p->infix)
        goto ESM_FETCH_VALUE;                /* Infix expression predicted => go get the operator */

    if (--p->call->remaining > 0)
        goto ESM_FETCH_VALUE;                /* Not enough arguments to trigger a call */

    if (TYPE(p->call->func) == TYPE_SET_WORD)
        goto ESM_EXEC_SET;

    if (TYPE(p->call->func) == TYPE_SET_PATH)
        goto ESM_EXEC_SETPATH;

    goto ESM_EXEC_CALL;                        


ESM_EXEC_CALL:
            
    if (!TypeCheckArgs(p))
    {
        if (TYPE(p->value) != TYPE_ERROR)
            p->value = NewError(1, 14, p->exec->blk);
        goto ESM_RAISE_ERROR; 
    }
    switch (TYPE(p->call->func))
    {
        case TYPE_NATIVE: 
        case TYPE_OP:        goto ESM_EXEC_NATIVE;
        case TYPE_ACTION:    goto ESM_EXEC_ACTION;
        case TYPE_FUNCTION: 
            p->value = FUNCTION(p->call->func)->body;    
            goto ESM_EXEC_DO;
    }

    
ESM_EXEC_DO:

    p->exec += 1;
    p->exec->ident = ACTION_FUNC;
    p->exec->blk = p->value;
    p->exec->idx = BLOCK(p->value)->head;
    p->exec->on_expr_done = NULL;
    p->exec->result = NULL;
    p->exec->base = p->call;
    goto ESM_FETCH_VALUE;


ESM_EXEC_ACTION:

    if (p->call->refs_idx)                                    /* test refinements presence */
        ret = ACTION(p->call->func)->body(
            p,                                                /* VTASK */
            p->values + p->call->args_idx,                    /* args */
            p->values + p->call->refs_idx    /* refs */
        );
    else
        ret = ACTION(p->call->func)->body(
            p,                                                /* VTASK */
            p->values + p->call->args_idx,                    /* args */
            NULL                                            /* No refinements */
        );
    switch ((ULONG)ret)
    {
        case (ULONG)GO_POP_DO:        goto ESM_POP_DO;
        case (ULONG)GO_POP_CALL:    goto ESM_POP_CALL;
        case (ULONG)GO_FETCH:        goto ESM_FETCH_VALUE;
    }
        

ESM_EXEC_NATIVE:

    if (p->call->refs_idx)                                    /* test refinements presence */
        p->value = NATIVE(p->call->func)->body(
            p->values + p->call->args_idx ,                    /* args */
            (VALUE *) p->values + p->call->refs_idx            /* refs */
        );
    else
        p->value = NATIVE(p->call->func)->body(
            p->values + p->call->args_idx,                    /* args */
            NULL                                            /* no refinements */
        );

    if (TYPE(p->value) == TYPE_NATIVE)            /* If result is native!, reuse the call slot */
    {
        p->path = NULL;
        p->call->args_idx += ANYFUNC(p->call->func)->totalcnt;    /* Jump over the previous args */
        p->call->func = p->value;        
        p->call->remaining = NATIVE(p->value)->totalcnt = NATIVE(p->value)->argcount;
        goto ESM_FETCH_VALUE;
    }
    goto ESM_POP_CALL;


ESM_EXEC_SET:

    p->word = WORD(p->call->func);
    p->value = SetContextDef(p->word->ctx, p->word, p->value);
    goto ESM_POP_CALL;


ESM_EXEC_SETPATH:

    ret = PathEval(p, PATH(p->call->func), 1);
    if (ret == GO_POP_CALL)
        goto ESM_POP_CALL;
    else
        goto ESM_RAISE_ERROR;


ESM_POP_DO:

    p->call = p->exec->base;
    p->exec -= 1;
    if (p->call != p->exec->base)
        goto ESM_POP_CALL;
    else
        goto ESM_PUSH_VALUE;


ESM_POP_CALL:

    while (p->call->remaining == 0)
    {
        p->top = (p->call->refs_idx ? p->call->refs_idx : p->call->args_idx) - 1;
        p->call -= 1;
        if (p->call == p->exec->base)
            break;
    }
    goto ESM_PUSH_VALUE;


ESM_EXPR_DONE:

    OnExprDone = p->exec->on_expr_done;
    if (OnExprDone)
    {
        ret = OnExprDone(p,
                p->values + p->exec->base->args_idx,
                p->values + p->exec->base->refs_idx);

    }
    
    p->exec->result = p->value;
    goto ESM_FETCH_VALUE;


ESM_RAISE_ERROR:

    while (p->exec != p->funcs)
    {
        if (p->exec->ident == ACTION_TRY)
            goto ESM_POP_DO;
        p->exec -= 1;
    }
    p->exec->result = p->value;
    /* goto ESM_POP_DO; */


ESM_FINISH:

    if (p->exec->result == NULL)
        p->exec->result = unset_value;
    else
    {
        if (TYPE(p->exec->result) != TYPE_ERROR)
            if ((p->call != p->exec->base) && (p->call->remaining > 0))
                p->exec->result = NewError(1, 8, p->exec->blk);        /* Missing argument */
    } 
}


VALUE Evaluate(VTASK task, VBLOCK code)
{
    InitTask(task, code);

    EvalStateMachine(task);

/*
    #if MEM_TEST == 1
        MemTest();
    #endif

    CheckGC();
*/    
    return task->exec->result;                            /* Returns last value */
}


VALUE Execute(VALUE p)
{
    switch(TYPE(p))
    {
        case TYPE_BLOCK: 
            return Evaluate(main_task, p);

        case TYPE_STRING: 
            p = Tokenize(STRING(p)->data, NULL);
            _ASSERT(TYPE(p) != TYPE_ERROR);
            return Evaluate(main_task, p);

        default: return p;
    }
}

VALUE RS_Mold(VALUE *args, VALUE *refs)
{
    VSTRING p;
    char *molded;
    UBYTE save;

    save = 0;
    molded = VTABLE(args[0])->Mold(args[0], &save);
    p = NewString(molded, -1);
    if (save)
        p->save = 1;                    /* Don't free the buffer with the string value */
    return (VALUE)p;
}


VALUE RS_Form(VALUE *args, VALUE *refs)
{
    VSTRING p;
    char *formed;
    UBYTE save;

    save = 0;
    formed = VTABLE(args[0])->Form(args[0], &save);
    p = NewString(formed, -1);
    if (save)                            
        p->save = 1;                    /* Don't free the buffer with the string value */
    return (VALUE)p;
}
