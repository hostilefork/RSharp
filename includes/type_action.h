#ifndef _ACTION_H
#define _ACTION_H

#define GO_FETCH        (void *)0
#define GO_EXEC_DO        (void *)1
#define GO_POP_DO        (void *)2
#define GO_POP_CALL        (void *)3
#define GO_PUSH_CALL    (void *)4
#define GO_PUSH_VALUE    (void *)5
#define GO_ERROR        (void *)6

struct VALUE_ACTION {
    HEADER32
    VBLOCK        specs;            /* Function definition block */
    VBLOCK        locals;            /* List of words to define locally (including refinements) */
    UWORD        argcount;        /* Minimum argument number */
    UWORD        totalcnt;        /* Total argument number accounting for refinements */
    UBYTE        **args;            /* Arguments type bitset list */
    VREFINEMENT    refines;        /* Optionnal refines with their argument list */
    ACTIONPTR    body;            /* Native C function to execute */
};

typedef struct VALUE_ACTION *VACTION;

VALUE    NewAction(VBLOCK);
void    MakeAllActions(VCONTEXT);
char    *MoldAction(VALUE, UBYTE *);
char    *FormAction(VALUE, UBYTE *);
char    CompareAction(VALUE, VALUE);

VALUE    RS_Do(VTASK, VALUE *, VALUE *);
VALUE    RS_Try(VTASK, VALUE *, VALUE *);
VALUE    RS_Reduce(VTASK, VALUE *, VALUE *);
VALUE    RS_If(VTASK, VALUE *, VALUE *);
VALUE    RS_Either(VTASK, VALUE *, VALUE *);
VALUE    RS_Loop(VTASK, VALUE *, VALUE *);
VALUE    RS_Exit(VTASK, VALUE *, VALUE *);
VALUE    RS_Return(VTASK, VALUE *, VALUE *);
VALUE    RS_Break(VTASK, VALUE *, VALUE *);
VALUE    RS_Catch(VTASK, VALUE *, VALUE *);
VALUE    RS_Throw(VTASK, VALUE *, VALUE *);
VALUE    RS_Any(VTASK, VALUE *, VALUE *);
VALUE    RS_All(VTASK, VALUE *, VALUE *);
VALUE    RS_Until(VTASK, VALUE *, VALUE *);
VALUE    RS_While(VTASK, VALUE *, VALUE *);

VALUE    ReduceNext(VTASK, VALUE *, VALUE *);
VALUE    LoopNext(VTASK, VALUE *, VALUE *);
VALUE    AnyNext(VTASK, VALUE *, VALUE *);
VALUE    AllNext(VTASK, VALUE *, VALUE *);
VALUE    UntilNext(VTASK, VALUE *, VALUE *);
VALUE    WhileNext(VTASK, VALUE *, VALUE *);


#endif
