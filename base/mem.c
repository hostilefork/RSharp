#include "rsharp.h"


/*=== VALUES memory management ===*/


void FormatValueFrame(SFRMV *frm)
{
    ULONG i;
    void **p;

    p = frm->stack;
    for(i = 0; i < MAX_VALUES; i++, p++)
    {
        *p = (void *)((char *)frm->base + ((MAX_VALUES - 1) * max_value_size) - i * max_value_size);
        **((ULONG **)p) = 0L;                /* Reset value header */
    }
}

void *AllocValueFrame()
{
    SFRMV *p, *last;

    p = malloc(FRM_VALUE_SIZE);
    if (p == NULL)
        return NULL;                        /* Init error : cannot allocated 1st frame ! */

    last = mem.values.head;
    if (last)
    {
        while (last->next != NULL) 
            last = last->next;
        last->next = p;
    }
    else
        mem.values.head = p;

    p->next = NULL;
    p->stack = (void *)((char *)p + struct_frame_value_size);
    p->base = (void *)((char *)p->stack + MAX_VALUES * sizeof(void *));
    p->top = (void **)((char *)p->base - 4);
    
    mem.values.active = p;
    FormatValueFrame(p);                    /* Fill the stack with cells address */


    return p;
}

void *InitValueMem()
{
    struct_frame_value_size = sizeof(struct frame_value);
    max_value_size = sizeof(struct VALUE_FUNCTION);            /* => !! should be a multiple of 8 !! */

    mem.values.head = NULL;
    mem.values.active = NULL;

    AllocValueFrame();
    return mem.values.head;
}

void *AllocValue()
{
    void *p;

    #if DEBUG_MEM == 1
        printf("mem : alloc value \n");
    #endif 

    if ((void *)mem.values.active->top == mem.values.active->stack)
        AllocValueFrame();                    /* alloc a new frame */

    p = *mem.values.active->top;
    HEADER(p)->mark = 0;
    mem.values.active->top--;                /* take next free slot */
    return p;
}

void *CloneValue(VALUE p)
{
    VALUE n;

    n = AllocValue();
    memcpy(n, p, max_value_size);
    return n;
}

/*=== Raw DATA memory management ===*/


void *AllocMem(ULONG size)
{
    void *p;

    p = malloc(size + MBH_SIZE);
    if (p == NULL)
        printf("*** Out of memory\n");

    #if DEBUG_MEM == 1
        printf("mem : alloc data size   = %lu\n", size);
    #endif 
    
    SBH(p)->size = size;
    SBH(p)->mark = 0;
    SBH(p)->save = 0;

    if (mem.data.head)
    {
        SBH(p)->prev = mem.data.tail;
        SBH(p)->next = mem.data.tail->next;
        mem.data.tail->next = SBH(p);
        mem.data.tail = SBH(p);
    }
    else
    {
        mem.data.head = mem.data.tail = SBH(p);
        SBH(p)->prev = NULL;
        SBH(p)->next = NULL;
    }

    return GET_MB_DATA(p);
}

void *AllocMemForever(ULONG size)
{
    void *data;

    data = AllocMem(size);
    SBH(GET_MB_HEAD(data))->save = 1;
    return data;
}

void FreeMem(void *data)
{
    MBH *p;

    p = GET_MB_HEAD(data);

    #if DEBUG_MEM == 1
        printf("mem : freeing data size = %lu\n", p->size);
    #endif 
    
    if (p->prev)
        p->prev->next = p->next;
    else
        mem.data.head = p->next;

    if (p->next)
        p->next->prev = p->prev;
    else
        mem.data.tail = p->prev;

    free((void *)p);
}

void *ReAllocMem(void *p, ULONG size)
{
    MBH *mb_prev, *mb_next, *n;

    n = GET_MB_HEAD(p);

    #if DEBUG_MEM == 1
        printf("mem : expanding  %lu => %lu\n", n->size, size);
    #endif 

    mb_prev = n->prev;
    mb_next = n->next;
    n = realloc(n, size + MBH_SIZE);
    if (n == NULL)
        printf("*** Out of memory\n");

    n->size = size;

    if (mb_prev)
        mb_prev->next = n;
    else
        mem.data.head = n;

    if (mb_next)
        mb_next->prev = n;
    else
        mem.data.tail = n;

    return GET_MB_DATA(n);
}

/*----------- Garbage Collector ------------*/

void MarkValue(VALUE);

void MarkBlock(VBLOCK blk)
{
    ULONG i;

    SBH(GET_MB_HEAD(blk->data))->mark = 1;

    for (i = 0; i < blk->tail; i++)
        MarkValue(blk->data[i]);
}

void MarkValue(VALUE p)
{
    VREFINEMENT ref;
    UWORD i;

    if (HEADER(p)->mark)
        return;

    HEADER(p)->mark = 1;

    if (IS_ANY_BLOCK(p))
    {
        MarkBlock(BLOCK(p));
        return;
    }
    if (IS_ANY_STRING(p))
    {
        SBH(GET_MB_HEAD(STRING(p)->data))->mark = 1;
        return;
    }
    if (TYPE(p) == TYPE_CONTEXT)
    {
        SBH(GET_MB_HEAD(CONTEXT(p)->table))->mark = 1;
        return;
    }
    if (TYPE(p) == TYPE_ERROR)
    {
        SBH(GET_MB_HEAD(ERROR(p)->message))->mark = 1;
        return;
    }
    if (IS_ANY_FUNC(p))
    {
        HEADER(ANYFUNC(p)->specs)->mark = 1;
        MarkBlock(ANYFUNC(p)->specs);

        if (ANYFUNC(p)->locals)
        {
            HEADER(ANYFUNC(p)->locals)->mark = 1;
            MarkBlock(ANYFUNC(p)->locals);
        }
        ref = ANYFUNC(p)->refines;
        if (ref)
        {
            while (ref)
            {
                ref->mark = 1;
                ref->word->mark = 1;
                if (ref->argcount)
                {
                    for(i = 0; i < ref->argcount; i++)
                        SBH(GET_MB_HEAD(ref->args[i]))->mark = 1;
                    SBH(GET_MB_HEAD(ref->args))->mark = 1;
                }
                ref = ref->next;
            }
        }
        if (TYPE(p) == TYPE_FUNCTION)
        {
            HEADER(FUNCTION(p)->body)->mark = 1;
            MarkBlock(BLOCK(FUNCTION(p)->body));
            FUNCTION(p)->ctx->mark = 1;
        }
        if (ANYFUNC(p)->argcount)
        {
            for(i = 0; i < ANYFUNC(p)->argcount; i++)
                SBH(GET_MB_HEAD(ANYFUNC(p)->args[i]))->mark = 1;
            SBH(GET_MB_HEAD(ANYFUNC(p)->args))->mark = 1;
        }
        return;
    }
    if (IS_ANY_WORD(p) && (TYPE(p) != TYPE_REFINEMENT))
        MarkValue(WORD(p)->ctx);
}

void CleanValueFrame(SFRMV *frm)
{
    ULONG **top;

    for (top = (ULONG **)frm->top; (void *)top >= frm->stack; top--)
        **top = (ULONG)0;
}

void SweepValues()
{
    SFRMV *frm;
    ULONG i;
    void *p;
    ULONG **top;

    frm = mem.values.active;

    top = frm->stack;
    for(i = 0; i < MAX_VALUES; i++)
    {
        p = (void *)((char *)frm->base + ((MAX_VALUES - 1) * max_value_size) - i * max_value_size);
        if (!HEADER(p)->mark)
        {
            *top = p;
/*
            if (VTABLE(p)->Free)
                VTABLE(p)->Free(p);
*/
            **top = (ULONG)0;
            top++;
        }
        else
            HEADER(p)->mark = 0;
    }
    frm->top = (void **)top;
}

void SweepData()
{
    MBH    *p, *q;

    p = mem.data.head;
    while (p)
    {
        if (!p->mark && !p->save)
        {
            q = p->next;
            FreeMem(GET_MB_DATA(p));
            p = q;
        }
        else
        {
            p->mark = 0;
            p = p->next;
        }
    }
}

void GarbageCollect()
{
    struct ctx_def *table;
    ULONG i;

/* DumpValueFrame(mem.values.active); */

    /*--- Mark phase---*/

    MarkValue(global_ctx);

    table = global_ctx->table;
    for (i = 0; i < global_ctx->tail ; i++)
    {
        if (table[i].value)
            MarkValue(table[i].value);
    }

    MarkValue(main_task->exec->blk);
    if (main_task->value)
        MarkValue(main_task->value);

    MarkValue(function_methods.NativeMakeType);
    MarkValue(op_methods.NativeMakeType);

    /*--- Sweep phase---*/

    CleanValueFrame(mem.values.active);
    SweepValues();
    SweepData();

/* DumpValueFrame(mem.values.active); */
}

void CheckGC()
{
/*    GarbageCollect(); */
}

/*======= Global functions & natives =======*/

void *InitMem()
{
    InitValueMem();

    return mem.values.active;
}

void FreeAllMem()
{
    SFRMV *vfrm, *v;
    MBH *p, *q;

    FreeTask(main_task);
    FreeSymbolsTable();

    vfrm = mem.values.head;
    while (vfrm)
    {
        v = vfrm->next;
        free(vfrm);
        vfrm = v;
    }

    p = mem.data.head;
    while (p)
    {
        q = p->next;
        free(p);
        p = q;
    }

}

VALUE RS_Recycle(VALUE *args, VALUE *refs)
{
    GarbageCollect();
    return unset_value;
}

VALUE RS_Stats(VALUE *args, VALUE *refs)
{
    ULONG cnt, total;
    SFRMV *vfrm;
    MBH *p;

    printf("\n--VALUE info--\n");
    cnt = ((char *)mem.values.active->base - (char *)mem.values.active->top) / sizeof(void *) + 1;
    printf("Active Frame Values\t= %lu/%lu\n", cnt, MAX_VALUES);

    cnt = total = 0;
    vfrm = mem.values.head;
    while (vfrm)
    {
        cnt += ((char *)vfrm->base - (char *)vfrm->top) / sizeof(void *) + 1;
        total += MAX_VALUES;
        vfrm = vfrm->next;
    }
    printf("Values\t\t\t= %lu/%lu\n", cnt, total);

    printf("\n--DATA info--\n");
    cnt = total = 0;
    p = mem.data.head;
    while (p) 
    { 
        total += p->size;
        cnt++;
        p = p->next;
    }
    printf("Allocated blocks\t= %lu\n", cnt);
    printf("Total bytes\t\t= %lu\n", total);

    return unset_value;
}
