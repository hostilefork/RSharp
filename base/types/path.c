#include "rsharp.h"


struct type_methods path_methods = {
    NULL,
    MoldPath,
    MoldPath,        /* Form */
    NULL,            /* Add */
    FreePath,
    NULL,            /* Subtract */
    NULL,            /* Multiply */
    NULL,            /* Divide */
    PickPath,
    CompareBlock,
    PathEvalBlock,
    SetPathBlock
};


VALUE NewPath(int size)
{
    VBLOCK p;

    p = NewBlock(size);
    p->type = TYPE_PATH;
    return (VALUE) p;
}

VALUE AltNewPath(VPATH model)
{
    VPATH p;

    p = NewPath(model->slots);
    memcpy(p, model, max_value_size);
    return (VALUE) p;
}

void FreePath(VALUE p)
{
    FreeBlock(BLOCK(p));
}

int AppendPath(VPATH p, VALUE val)
{
    return AppendBlock(BLOCK(p), val);
}

VALUE PickPath(VALUE p, long idx)
{
    return PickBlock(p,idx);
}

char *MoldPath(VALUE p, UBYTE *save)
{
    VPATH path;
    char *s, *item;
    ULONG i, size;
    UBYTE lit;

    _ASSERT(TYPE(p) == TYPE_PATH || TYPE(p) == TYPE_LIT_PATH || TYPE(p) == TYPE_SET_PATH);

    path = PATH(p);
    lit = (TYPE(p) == TYPE_LIT_PATH) ? 1 : 0;
    size = 64;
    s = (char *) AllocMem(size);
    s[0] = '\0';
    if (path->head < path->tail)
    {
        for (i = path->head; i < path->tail; i++)
        {
            item = VTABLE(path->data[i])->Mold(path->data[i], save);
            if ((strlen(item) + strlen(s) + 2)> size)
            {
                size = 2 * (ULONG)(strlen(item) + strlen(s) + 2);
                s = ReAllocMem(s, size);
            }
            strcat(s, item);
            if (i < path->tail - 1)
                strcat(s, "/");
        }
        if (TYPE(p) == TYPE_SET_PATH)
            strcat(s, ":");
    }
    return s;
}

