#include "rsharp.h"

/* For debugging only */

char *types_name[] = {
    "block!", "word!", "set-word!", "get-word!", "lit-word!", "integer!", "decimal!", "string!",
    "char!", "bitset!"
};

void DumpBlock(VBLOCK p)
{
    VHEADER vh;
    UBYTE    i;

    printf("[");
    for(i = 0; i < p->tail; i++)
    {
        vh = (VHEADER) p->data[i];
        if (vh->type == TYPE_BLOCK)
            DumpBlock(p->data[i]);
        else
            printf("%s", types_name[vh->type - 1]);
        if (i < p->tail - 1) printf(" ");
    }
    printf("]");
}

void DumpContext(VCONTEXT ctx)
{
    ULONG i;

    printf("\n##Context Dump :\n");
    for (i = 0; i < ctx->tail; i++)
        printf("%s ", GetSymbol(ctx->table[i].symbol));
}


void DumpValueFrame(SFRMV *frm)
{
    FILE *fp;
    ULONG i;
    VALUE val;
    UBYTE flag;
    char name[80];
    static ULONG cnt;

    sprintf(name, "values%00lu.log", cnt++);
    fp = fopen(name, "w");

    fprintf(fp, "\n====Début====\n");
    for(i = 0; i < MAX_VALUES; i++)
    {
        val = (void *)((char *)frm->base + i * max_value_size);
        if (TYPE(val) > 0)
        {
            fprintf(fp, "%u\t: %u, %s", i, HEADER(val)->mark, GetTypeName(val));
            if (VTABLE(val)->Mold)
                fprintf(fp, ", %s",VTABLE(val)->Mold(val, &flag));
            fprintf(fp, "\n");
        }
    }
    fprintf(fp, "=============\n");
    fclose(fp);
}
