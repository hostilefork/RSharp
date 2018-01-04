#include "rsharp.h"

struct type_methods string_methods = {
    NULL,
    MoldString,
    FormString,
    NULL,            /* Add */
    FreeString,
    NULL,            /* Subtract */
    NULL,            /* Multiply */
    NULL,            /* Divide */
    PickString,
    CompareString,
    PathEvalString,
    SetPathString
};


VALUE NewString(char *start, int len)
{
    VSTRING p;
    ULONG size;

    p = (VSTRING) AllocValue();
    p->type = TYPE_STRING;
    p->save = 0;
    p->reserved = 0;
    if (len == -1)                        /* len = -1 => do not copy start buffer */
    {
        p->data = start;
        p->slots = 0;
        if (*start)
            while(start[p->slots]) p->slots++;
        p->tail = p->slots;
        p->slots++;
    }
    else
    {
        size = ROUND16(len);    /* size increased to the next superior multiple of 16 */
        p->data = AllocMem(size);
        if (len)
            strncpy(p->data, start, len);
        p->slots = size;
        p->tail = len;
    }
    p->head = 0;
    p->save = 0;
    return((VALUE) p);
}

void FreeString(VALUE p)
{
    if (!STRING(p)->save)
        FreeMem(STRING(p)->data);
}

void ExpandString(VSTRING p, int size)
{
    p->slots = ROUND16((p->slots + size + ((size) ? size : 16)));
    p->data = ReAllocMem(p->data, p->slots);
}

VALUE PathEvalString(VALUE p, VALUE value)
{
    if (TYPE(value) == TYPE_GET_WORD)
        value = GetValueWord(WORD(value));

    if (TYPE(value) == TYPE_INTEGER)
        return PickString(BLOCK(p), INTEGER(value)->data);

    return NewError(1, 12, NULL);
}

VALUE SetPathString(VALUE str, VALUE arg, VALUE value)
{
    if ((TYPE(arg) != TYPE_INTEGER) && (TYPE(arg) != TYPE_LOGIC))
        return NewError(1, 29, NULL);

    return PokeString(str, arg, value); 
}

VALUE InsertString(VALUE *arg, VALUE *ref)
{
    VSTRING str;
    VINTEGER count;
    VALUE range;
    char *value;
    ULONG cnt, size, tmp, total, start, j, value_head;
    UBYTE flag;
    UWORD i;

    range = NULL;
    count = NULL;
    cnt = 0;

    /* Arguments processing */
    switch (TYPE(arg[1]))
    {
        case TYPE_STRING:
            str = STRING(arg[1]);
            value_head = str->head;
            value = str->data;
            size = str->tail - value_head;
            break;

        case TYPE_CHAR:
            value_head = 0;
            value = (char *)&CHAR(arg[1])->data;
            size = 1;
            break;
                                                /* A Faire: si c un ANY_BLOCK => parcourir la liste ! */
        default:
            value_head = 0;
            value = VTABLE(arg[1])->Form(arg[1], &flag);
            size = strlen(value);
    }

    str = STRING(arg[0]);
    total = str->tail + size;

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

                case REF_INSERT_DUP:
                    count = arg[2 + i];
                    cnt = MAX(INTEGER(count)->data, 0);
                    break;
            }
        }
        if (range)
        {
            if (TYPE(range) == TYPE_INTEGER)                    /* to be changed to IS_NUMBER(range) */
            {
                tmp = MAX(0, INTEGER(range)->data);
                size = MIN(size, tmp);
            }
            else
                if (TYPE(range) == TYPE_STRING)
                    size = MIN(size, STRING(range)->head);
                else
                    return (VALUE) NewError(1, 15, NULL);        /* Invalid part count argument */
            total = str->tail + size;
        }

        if (count)
            total = str->tail + size * cnt;
    }

    if (str->slots < total)
    {
        tmp = (str->data == value);
        ExpandString(str, total);
        if (tmp)
            value = str->data;
    }

    total -= str->tail;

    if (str->head == str->tail)
        start = str->tail;
    else
    {
        memmove(
            str->data + str->head + total,
            str->data + str->head,
            str->tail - str->head);
        start = str->head;
    }
    if (count)
        for(j = 0; j < cnt; j++)                                /* Should be optimized */
            memcpy(str->data + start + j * size, value + value_head, size);
    else
        memcpy(str->data + start, value + value_head, size);

    str->head += total;
    str->tail += total;

    return (VALUE) str;
}

VALUE CopyString(VALUE *args, VALUE *refs)
{
    VSTRING nstr, ostr;
    long part;
    ULONG nsize;
    UBYTE deep;
    UWORD i;
    VALUE *part_arg;

    deep = 0;
    part_arg = NULL;

    ostr = STRING(args[0]);
    if (refs)
    {
        for(i = 0; refs + i < args; i++)
        {
            if ((ULONG)refs[i] == REF_COPY_PART)
            {
                part_arg = args[1 + i];
                switch(TYPE(part_arg))
                {
                case TYPE_INTEGER:
                    part = INTEGER(part_arg)->data;
                    break;
                case TYPE_STRING:
                    if (STRING(part_arg)->data == STRING(args[0])->data)
                    {
                        part = STRING(part_arg)->head - ostr->head;
                        break;
                    }
                default:
                    return NewError(1, 24, NULL);
                }
            }
        }
    }
    if (!part_arg)
    {
        part = ostr->tail - ostr->head;
        nsize = ostr->slots; 
    }
    else
    {
        part = MIN((ostr->tail - ostr->head), (ULONG)(MAX(part, 0)));
        nsize = ROUND16(part);
    }

    nstr = AllocValue();
    memcpy(nstr, args[0], sizeof(struct VALUE_STRING));
    nstr->data = AllocMem(nsize);
    nstr->save = 0;
    nstr->slots = nsize;
    nstr->head = 0;
    nstr->tail = part;
    memcpy(nstr->data, (char *)ostr->data + ostr->head, part);
    return (VALUE) nstr;
}

char CompareStringCase(VALUE strA, VALUE strB)
{
    ULONG sizeA, sizeB;

    sizeA = STRING(strA)->tail - STRING(strA)->head;
    sizeB = STRING(strB)->tail - STRING(strB)->head;
    if (sizeA == sizeB)
        return strncmp(STRING(strA)->data, STRING(strB)->data, sizeA);
    else
        return sizeA < sizeB ? 1 : -1;
}

char CompareString(VALUE str, VALUE value)
{
    ULONG sizeA, sizeB;

    if (IS_ANY_STRING(value))
    {
        sizeA = STRING(str)->tail - STRING(str)->head;
        sizeB = STRING(value)->tail - STRING(value)->head;
        if (sizeA == sizeB)
            return strncmpi(STRING(str)->data, STRING(value)->data, sizeA);
        else
            return sizeA < sizeB ? 1 : -1;
    }
    else
        return IS_ANY_BLOCK(value) ? 1 : -1;        /* string! < block! */
}

VALUE PickString(VALUE str, long idx)
{
    idx += STRING(str)->head;
    if (idx > 0)
        idx--;

    if ((ULONG)idx >= 0 && (ULONG)idx < STRING(str)->tail)
        return (VALUE) NewChar(STRING(str)->data[idx], 0);
    else
        return none_value;
}

VALUE PokeString(VALUE str, VALUE index, VALUE c)
{
    ULONG idx;

    if (TYPE(c) != TYPE_CHAR)
        return NewError(1, 11, NULL);

    idx = INTEGER(index)->data + STRING(str)->head - 1;

    if ((ULONG)idx >= 0 && (ULONG)idx < STRING(str)->tail)
    {
        STRING(str)->data[idx] = CHAR(c)->data;
        return str;
    }
    else
        return NewError(1, 28, NULL);
}

/* ---- Volker Nitsch contribution (added 26/08/2003) ---- */

/* /// Manque les caractères accentués !!! /// */

VALUE ChangeCase( VALUE *args, VALUE *refs, bool lower_else_upper )
{
    VSTRING    str;
    ULONG    i,size;
    char    *at;

    _ASSERT(IS_ANY_STRING(args[0]));
    _ASSERT(!refs || (ULONG)refs[0] == REF_LOWUPCASE_PART);

    str = STRING(args[0]);
    at = & str->data[str->head];
    size = str->tail - str->head;

    if (refs)
        size = MIN(size, (ULONG)MAX(0,INTEGER(args[1])->data));

    if (lower_else_upper) 
    {
        for(i=0;i < size; i++)
            if( at[i] >= 'A' && at[i] <= 'Z' )
                at[i] += 32;                /* c - 'A' + 'a'; */
    }
    else
    {
        for(i=0;i < size; i++)
            if( at[i] >= 'a' && at[i] <= 'z' )
                at[i] -= 32;                /* c - 'a' + 'A'; */
    }
    return (VALUE) str;
}

VALUE RS_Lowercase(VALUE *args, VALUE *refs)
{
    return ChangeCase(args,refs,true);
}

VALUE RS_Uppercase(VALUE *args, VALUE *refs)
{
    return ChangeCase(args,refs,false);
}  
/* ---- VN ---- */

char *MoldString(VALUE p, UBYTE *save)
{
    VSTRING str;
    char *s, quote;
    ULONG size;

    _ASSERT(TYPE(p) == TYPE_STRING);

    str = (VSTRING) p;
    size = str->tail - str->head;
    s = (char *) AllocMem(size + 3);
    quote = '"';
    if (size)
        strncpy(s + 1, str->data + str->head, size);
    s[0] = quote;
    s[1 + size] = quote;
    s[2 + size] = '\0';
    return s;
}

char *FormString(VALUE p, UBYTE *save)
{
    VSTRING str;
    char *s;
    ULONG size;

    _ASSERT(TYPE(p) == TYPE_STRING);

    str = (VSTRING) p;
    size = str->tail - str->head;
    s = (char *) AllocMem(size + 1);
    s[0] = '\0';
    if (size)
    {
        strncpy(s, str->data + str->head, size);
        s[size] = '\0';
    }
    return s;
}
