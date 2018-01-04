
struct type_methods *methods[29] = {
    &unset_methods,
    &block_methods,
    &word_methods,
    &word_methods,        /* TYPE_SET_WORD */
    &word_methods,        /* TYPE_GET_WORD */
    &word_methods,         /* TYPE_LIT_WORD */
    &integer_methods,
    NULL,                /*    decimal_methods, */
    &string_methods,
    &char_methods,
    NULL,                /*    bitset_methods, */
    &error_methods,
    &native_methods,
    &context_methods,
    &none_methods,
    &logic_methods,
    &datatype_methods,
    &refinement_methods,
    &path_methods,
    &path_methods,        /* TYPE_SET_PATH */
    &path_methods,        /* TYPE_LIT_PATH */
    &function_methods,
    &op_methods,
    NULL,                /* TYPE_TUPLE */
    NULL,                /* TYPE_PORT */
    NULL,                /* TYPE_BINARY */
    &action_methods,
    &paren_methods
};

VCONTEXT global_ctx;        /* Global Context */
VTASK     main_task;            /* Main thread of execution */

/*char     *sym_local;    */    /* Reference to "local" symbol */
