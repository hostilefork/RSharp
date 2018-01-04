#include "rsharp.h"


struct type_methods block_methods = {
    NULL,
    MoldBlock,
    FormBlock,
    NULL,                /* Add */
    FreeBlock,
    NULL,                /* Subtract */
    NULL,                /* Multiply */
    NULL,                /* Divide */
    PickBlock,
    CompareBlock,
    PathEvalBlock,
    SetPathBlock
};


VALUE NewBlock(int size)
{
    VBLOCK p;

    p = (VBLOCK) AllocValue();
    p->type = TYPE_BLOCK;
    p->save = 0;
    p->reserved = 0;
    size = ROUND16(size);    /* size increased to the next closest multiple of 16 */
    p->data = AllocMem(size * sizeof(VALUE));
    p->slots = size;
    p->tail = 0;
    p->head = 0;
    return((VALUE) p);
}

void FreeBlock(VALUE p)
{
    FreeMem(BLOCK(p)->data);
}

void ExpandBlock(VBLOCK p, int size)
{
    size = ROUND16((size ? size : p->slots + 16) * sizeof(VALUE));
    p->data = ReAllocMem(p->data, size);
    p->slots = size / sizeof(VALUE);
}

int AppendBlock(VBLOCK p, VALUE val)
{
    _ASSERT(p->tail <= p->slots);

    if (p->slots == p->tail)
        ExpandBlock(p, 0);

    p->data[p->tail++] = val;

    return p->tail;
}

VALUE PathEvalBlock(VALUE p, VALUE value)
{
    if (TYPE(value) == TYPE_GET_WORD)
        value = GetValueWord(WORD(value));

    if (TYPE(value) == TYPE_INTEGER)
        return PickBlock(BLOCK(p), INTEGER(value)->data);

    return NewError(1, 12, NULL);
}

VALUE SetPathBlock(VALUE blk, VALUE arg, VALUE value)
{
    if ((TYPE(arg) != TYPE_INTEGER) && (TYPE(arg) != TYPE_LOGIC))
        return NewError(1, 29, NULL);

    return PokeBlock(blk, arg, value);
}

VALUE InsertBlock(VALUE *arg, VALUE *ref)
{
    VBLOCK blk;
    VINTEGER count;
    VALUE range;
    VALUE *values;
    ULONG size, tmp, total, start, value_head;
    UWORD i;

    range = NULL;
    count = NULL;

    /* Arguments processing */
    if (IS_ANY_BLOCK(arg[1]))
    {
        blk = BLOCK(arg[1]);                /* setting 'blk to arg1 (the value) */
        value_head = blk->head;
        values = blk->data;
        size = blk->tail - value_head;
    }
    else
    {
        value_head = 0;
        values = arg + 1;
        size = 1;
    }

    blk = BLOCK(arg[0]);                    /* resetting 'blk to arg0 (the series) */
    total = blk->tail + size;

    /* Refinement and optional arguments processing */
    if (ref)
    {
        for(i = 0; ref + i < arg; i++)
        {
            switch((ULONG)ref[i])
            {
                case REF_INSERT_PART:
                    range = arg[2 + i];
                    break;

                case REF_INSERT_ONLY:
                    value_head = 0;
                    values = arg + 1;
                    total = total - size + 1;
                    size = 1;
                    break;

                case REF_INSERT_DUP:
                    count = arg[2 + i];
                    break;
            }
        }
        if (range)
        {
            if (TYPE(range) == TYPE_INTEGER)                    /* to be changed to IS_ANY_NUMBER(range) */
            {
                tmp = MAX(0, INTEGER(range)->data);
                size = MIN(size, tmp);
            }
            else
                if (IS_ANY_BLOCK(range))
                    size = MIN(size, BLOCK(range)->head);
                else
                    return (VALUE) NewError(1, 15, NULL);        /* Invalid part count argument */
            total = blk->tail + size;
        }

        if (count)
            total = blk->tail + size * count->data;
    }

    if (blk->slots < total)
    {
        tmp = ((VALUE *)blk->data == values);            /* test if 'series and 'value are same */
        ExpandBlock(blk, total);                        /* 'value may refer to an invalid memory area, */
        if (tmp)                                        /* after the reallocation */
            values = (VALUE *)blk->data;                /* reset values to the reallocated area */
    }

    total -= blk->tail;

    if (blk->head == blk->tail)
        start = blk->tail;
    else
    {
        memmove(
            blk->data + blk->head + total, blk->data + blk->head,
            (blk->tail - blk->head) * sizeof(VALUE)
        );
        start = blk->head;
    }
    if (count)
    {
        for (i = 0; i < count->data; i++)                                /* Should be optimized */
            memcpy(blk->data + start + i * size,
                   values + value_head,
                   size * sizeof(VALUE));
    }
    else
        memcpy(blk->data + start, values + value_head, size * sizeof(VALUE));

    blk->head += total;
    blk->tail += total;

    return (VALUE) blk;
}

VALUE CopyBlock(VALUE *args, VALUE *refs)
{
    VBLOCK nblk, oblk;
    long part;
    ULONG nsize;
    UBYTE deep;
    UWORD i;
    VALUE *part_arg;

    deep = 0;
    part_arg = NULL;

    oblk = BLOCK(args[0]);
    if (refs)
    {
        for(i = 0; refs + i < args; i++)
        {
            switch((ULONG)refs[i])
            {
            case REF_COPY_DEEP:
                /*deep = 1; */
                break;

            case REF_COPY_PART:
                part_arg = args[1 + i];
                switch(TYPE(part_arg))
                {
                case TYPE_INTEGER:
                    part = INTEGER(part_arg)->data;
                    break;
                case TYPE_BLOCK:
                    if (BLOCK(part_arg)->data == BLOCK(args[0])->data)
                    {
                        part = BLOCK(part_arg)->head - oblk->head;
                        break;
                    }
                default:
                    return NewError(1, 24, NULL);
                }
                break;
            }
        }
    }
    if (!part_arg)
    {
        part = oblk->tail - oblk->head;
        nsize = oblk->slots; 
    }
    else
    {
        part = MIN((oblk->tail - oblk->head), (ULONG)(MAX(part, 0)));
        nsize = ROUND16(part);
    }

    nblk = AllocValue();
    memcpy(nblk, args[0], sizeof(struct VALUE_BLOCK));
    nblk->data = AllocMem(nsize * sizeof(VALUE));
    nblk->save = 0;
    nblk->slots = nsize;
    nblk->head = 0;
    nblk->tail = part;
    memcpy(nblk->data, (char *)oblk->data + (oblk->head * sizeof(VALUE)), part * sizeof(VALUE));
    return (VALUE) nblk;
}

char CompareBlock(VALUE p, VALUE value)
{
    VBLOCK a, b;
    ULONG i, size;
    char cmp;

    if (IS_ANY_BLOCK(value))
    {
        a = BLOCK(p);
        b = BLOCK(value);
        size = a->tail - a->head;
        if (size == (b->tail - b->head))
        {
            for(i = 0; i < size; i++)
            {
                cmp = VTABLE(a->data[a->head + i])->Compare(
                    a->data[a->head + i],
                    b->data[b->head + i]);
                if (cmp)
                    return cmp;
            }
            return 0;
        }
        else
            return size < (b->tail - b->head) ? 1 : -1;
    }
    else
        return -1;                            /* block! > value */
}

VALUE PickBlock(VALUE blk, long idx)
{
    idx += BLOCK(blk)->head;
    if (idx > 0)
        idx--;

    if ((ULONG)idx >= 0 && (ULONG)idx < BLOCK(blk)->tail)
        return (VALUE) BLOCK(blk)->data[idx];
    else
        return none_value;
}

VALUE PokeBlock(VALUE blk, VALUE index, VALUE value)
{
    ULONG idx;

    idx = INTEGER(index)->data + BLOCK(blk)->head - 1;

    if ((ULONG)idx >= 0 && (ULONG)idx < BLOCK(blk)->tail)
    {
        BLOCK(blk)->data[idx] = value;
        return blk;
    }
    else
        return NewError(1, 28, NULL);
}

VALUE FirstBlock(VBLOCK p)
{
    VALUE val;

    if (val = p->data[p->head])
        return (VALUE) val;
    else
        return (VALUE) NewError(1, 6, NULL);
}

char *Serialize(VALUE p, UBYTE mold)                /* mold = 1 => molded, 0 => formed */
{
    VBLOCK blk;
    char *s, *item;
    ULONG i, size;
    UBYTE save;

    blk = BLOCK(p);
    size = 64;
    s = (char *) AllocMem(size);
    s[0] = '\0';
    if (mold)
        strcat(s, "[");
    for (i = blk->head; i < blk->tail; i++)
    {
        if (mold)
            item = VTABLE(blk->data[i])->Mold(blk->data[i], &save);
        else
            item = VTABLE(blk->data[i])->Form(blk->data[i], &save);

        if ((strlen(item) + strlen(s) + 2)> size)
        {
            size = 2 * (ULONG)(strlen(item) + strlen(s) + 2);
            s = ReAllocMem(s, size);
        }

        strcat(s, item);
        if (i < blk->tail - 1)
            strcat(s, " ");
    }
    _ASSERT(strlen(s) + 2 < size);

    if (mold)
        strcat(s, "]");
    return s;
}


char *MoldBlock(VALUE p, UBYTE *save)
{
    return Serialize(p, 1);
}

char *FormBlock(VALUE p, UBYTE *save)
{
    return Serialize(p, 0);
}
