#include "rsharp.h"

char CheckError(VALUE value)
{
    VERROR    err;

    if (TYPE(value) == TYPE_ERROR)
    {
        err = (VERROR) value;
        printf("%s\n", err->message);
        return 1;
    }
    return 0;
}

void process(char *data)
{
    VALUE code, ret;

    code = Tokenize(data, NULL);
    if (CheckError(code))
        return;
    BindToContext(global_ctx, code);
    ret = Execute(code);

    if (CheckError(ret))
        return;
    if (IS_ANY_FUNC(ret) || TYPE(ret) == TYPE_UNSET)
        return;
    printf("== ");
    ret = RS_Mold(&ret, NULL);
    printf("%s\n", ((VSTRING)ret)->data);
}

void console(void)
{
   char buffer[1024];

   printf("\nEntering Console mode...\n");
   for(;;)
   {
        printf(">> ");
        fgets(buffer, 1024, stdin);    /* more secure than gets, overflow control | BHA 28/04/2003 */

        if (*buffer != '\n')        /* Test for non-empty entry */
            process(buffer);
   }
}

VALUE RS_Output(VALUE *args, VALUE *refs)
{
    VSTRING p;
    ULONG i;

    _ASSERT(TYPE(args[0]) == TYPE_STRING);

    p = STRING(args[0]);
    for(i = p->head; i < p->tail; i++)
        fputc(p->data[i], stdout);

    if (refs)
        printf("\n");

    return unset_value;
}
