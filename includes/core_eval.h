#ifndef _EVAL_H
#define _EVAL_H

#include "rsharp.h"

#define PUSH(x) p->values[++p->top] = x
#define POP()   p->values[--p->top]

#define FUNC_STK_SIZE    256

#define EXECSLOT        struct exec_slot
#define CALLSLOT        struct call_slot


#define NO_ACTION        NULL
#define ACTION_DO        1
#define ACTION_TRY        2
#define ACTION_CATCH    3
#define ACTION_LOOP        4
#define    ACTION_FUNC        5


struct VALUE_TASK;
typedef struct VALUE_TASK *VTASK;

typedef void *(*ACTIONPTR)(VTASK, void **, void **);    /* task, args, refs */

struct exec_slot {
    ULONG        ident;
    ACTIONPTR    on_expr_done;
    VBLOCK        blk;                /* Current execution block */
    ULONG        idx;                /* Index of currently evaluated element in exec_blk */
    VALUE        result;                /* for function! calls */
    CALLSLOT    *base;
    UWORD        locals_idx;            /* Local variables stack index (used by callbacks) */
    UWORD        padding;
};

struct call_slot {
    void        *func;
    ACTIONPTR    on_expr_done;
    VALUE        referer;
    UWORD        remaining;
    UWORD        args_idx;
    UWORD        refs_idx;
    UWORD        ctx_idx;
};

struct VALUE_TASK {
    UWORD        top;                /* Index of the values stack top element */
    UWORD        infix;                /* Temporary variable used in infix expression processing */
    VALUE        value;                /* Current value */
    VWORD        word;
    VPATH        path;
    EXECSLOT    *exec;                /* Current execution structure */
    CALLSLOT    *last;                /* Last slot in stack (for overflow detection) */
    CALLSLOT    *call;                /* Index of pending function/native/set-word to execute */
    EXECSLOT    *funcs;                /* Function calls stack */
    CALLSLOT    *calls;                /* Function calls stack */
    VALUE        *values;            /* Function arguments stack */
};


VTASK   NewTask();
void    InitTask(VTASK, VBLOCK);
void    FreeTask(VTASK);
void    InitInterpreter();


VALUE    ThrowException(VTASK, ULONG, UWORD);
VALUE   Execute(VALUE);
VSTRING Mold(VALUE);


VALUE   RS_Mold(VALUE *, VALUE *);
VALUE    RS_Form(VALUE *, VALUE *);

#endif
