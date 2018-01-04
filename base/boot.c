#include "rsharp.h"
#include "core_globals.h"

#define BBSIZE 65536

void LoadDefs(void)
{
    VALUE code;
    FILE *fp;
    char *buf, line[256];
    ULONG len, i;

    if ((fp = fopen("boot.rs", "r")) == NULL)
    {
        printf("\n** File: boot.rs not found!\n");
        return;
    }
    if ((buf = (char *) malloc(BBSIZE)) == NULL)
    {
        printf("\n** LoadDefs: cannot allocate buffer !\n");
        return;
    }
    i = 0;
    *buf = 0;
    while(!feof(fp))
    {
        len = fread(line, sizeof(char), 256, fp);
        strncat(buf, line, len);
        i += len;
    }
    fclose(fp);

    code = Tokenize(buf, NULL);
    free((void *)buf);

    if (CheckError(code))
        return;
    if (CheckError(Execute(code)))
        return;
}

int main(void)
{
    printf("--- R# v0.6.0 - Alpha ---\n");

    /* --- Booting --- */

    InitMem();
    global_ctx = NewContext(GLOBAL_CTX_SIZE);
    InitSymbolsTable();
    InitDatatypes();

    unset_value = &unset_struct_value;
    unset_value->type = TYPE_UNSET;

    none_value = &none_struct_value;
    none_value->type = TYPE_NONE;
    SetContextDef(global_ctx, NewWord("none", 4, TYPE_WORD), none_value);

    true_value = &true_logic_value;
    true_value->type = TYPE_LOGIC;
    true_value->data = 0;
    SetContextDef(global_ctx, NewWord("true", 4, TYPE_WORD), true_value);

    false_value = &false_logic_value;
    false_value->type = TYPE_LOGIC;
    false_value->data = 1;
    SetContextDef(global_ctx, NewWord("false", 5, TYPE_WORD), false_value);

    BuildNativeMakeFunction();
    BuildNativeMakeOp();
    MakeAllNatives(global_ctx);
    InitInterpreter();

    main_task = NewTask(&main_task);

    LoadDefs();

    RS_Stats(NULL, NULL);

/*    printf("function! => %ld\n", sizeof(struct VALUE_FUNCTION));*/

    console();
}
