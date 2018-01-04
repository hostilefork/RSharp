#ifndef _RSHARP_H
#define _RSHARP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#ifdef WIN32
    #include <conio.h>
    #include <crtdbg.h>
#else
    #define _ASSERT    assert
#endif


#define true    -1
#define false    0

#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)
#define ROUND16(x) ((((x) >> 4) + 1) << 4)        /* round to the next multiple of 16 */

#define TYPE_UNSET            0
#define TYPE_BLOCK            1
#define TYPE_WORD            2
#define TYPE_SET_WORD        3
#define TYPE_GET_WORD        4
#define TYPE_LIT_WORD        5
#define TYPE_INTEGER        6
#define TYPE_DECIMAL        7
#define TYPE_STRING            8
#define TYPE_CHAR            9
#define TYPE_BITSET            10
#define TYPE_ERROR            11
#define TYPE_NATIVE            12
#define TYPE_CONTEXT        13
#define TYPE_NONE            14
#define TYPE_LOGIC            15
#define TYPE_DATATYPE        16
#define    TYPE_REFINEMENT        17
#define    TYPE_PATH            18
#define    TYPE_SET_PATH        19
#define    TYPE_LIT_PATH        20
#define    TYPE_FUNCTION        21
#define    TYPE_OP                22
#define TYPE_TUPLE            23
#define TYPE_PORT            24
#define TYPE_BINARY            25
#define TYPE_ACTION            26
#define TYPE_PAREN            27

/* TYPE >= 200 => Virtual types */
#define TYPE_ANY_WORD        249
#define TYPE_ANY_STRING        250
#define TYPE_ANY_BLOCK        251
#define TYPE_NUMBER            252
#define    TYPE_ANY_FUNC        253
#define    TYPE_SERIES            254
#define TYPE_ANY            255

#define HEADER(x)        ((VHEADER)x)
#define LOGIC(x)        ((VLOGIC)x)
#define DATATYPE(x)        ((VDATATYPE)x)
#define SERIES(x)        ((VSERIES)x)
#define BLOCK(x)        ((VBLOCK)x)
#define STRING(x)        ((VSTRING)x)
#define CHAR(x)            ((VCHAR)x)
#define PATH(x)            ((VPATH)x)
#define INTEGER(x)        ((VINTEGER)x)
#define NATIVE(x)        ((VNATIVE)x)
#define WORD(x)            ((VWORD)x)
#define REFINEMENT(x)    ((VREFINEMENT)x)
#define ANYFUNC(x)        ((VANYFUNC)x)
#define FUNCTION(x)        ((VFUNCTION)x)
#define ACTION(x)        ((VACTION)x)
#define PAREN(x)        ((VPAREN)x)
#define ERROR(x)        ((VERROR)x)
#define CONTEXT(x)        ((VCONTEXT)x)

#define TYPE(x)            (*(VTYPE)x)            /* ((((VHEADER)x)->type) */
#define VTABLE(x)        methods[TYPE(x)]

#define IS_VIRTUAL_TYPE(x) (x > 200)
#define IS_NUMBER(x) (                \
    (TYPE(x) == TYPE_INTEGER) ||    \
    (TYPE(x) == TYPE_DECIMAL)        \
)
#define IS_SERIES(x) (                \
    (TYPE(x) == TYPE_BLOCK) ||        \
    (TYPE(x) == TYPE_STRING) ||        \
    (TYPE(x) == TYPE_PATH) ||        \
    (TYPE(x) == TYPE_SET_PATH) ||    \
    (TYPE(x) == TYPE_LIT_PATH) ||    \
    (TYPE(x) == TYPE_PAREN)            \
)
#define IS_ANY_WORD(x) (            \
    (TYPE(x) == TYPE_WORD) ||        \
    (TYPE(x) == TYPE_GET_WORD) ||    \
    (TYPE(x) == TYPE_SET_WORD) ||    \
    (TYPE(x) == TYPE_LIT_WORD) ||    \
    (TYPE(x) == TYPE_REFINEMENT)    \
)
#define IS_ANY_FUNC(x) (            \
    (TYPE(x) == TYPE_NATIVE) ||        \
    (TYPE(x) == TYPE_FUNCTION) ||    \
    (TYPE(x) == TYPE_OP) ||            \
    (TYPE(x) == TYPE_ACTION)        \
)
#define IS_ANY_STRING(x) (TYPE(x) == TYPE_STRING)

#define IS_ANY_BLOCK(x) (            \
    (TYPE(x) == TYPE_BLOCK) ||        \
    (TYPE(x) == TYPE_PATH) ||        \
    (TYPE(x) == TYPE_SET_PATH) ||    \
    (TYPE(x) == TYPE_LIT_PATH) ||    \
    (TYPE(x) == TYPE_PAREN)            \
)
#define IS_TYPE_MISC(x) (            \
    (TYPE(x) == TYPE_NONE) ||        \
    (TYPE(x) == TYPE_LOGIC) ||        \
    (TYPE(x) == TYPE_UNSET)            \
)
#define IN_BITSET(array,x)    array[x>>3] & 1<<(x%8)

#define HEADER32            \
    ULONG    type : 8;        \
    ULONG    mark : 1;        \
    ULONG    save : 1;        \
    ULONG    reserved : 22;    

typedef void *(*FUNCPTR)(void **, void **);            /* args, refs */
typedef void *VALUE;
typedef unsigned long        ULONG;
typedef    unsigned short int    UWORD;
typedef unsigned char        UBYTE;
typedef    unsigned char        bool;


struct VALUE_HEADER {
    HEADER32
};

typedef struct VALUE_HEADER *VHEADER;
typedef UBYTE *VTYPE;

#include "os.h"
#include "core_boot.h"
#include "core_mem.h"
#include "core_symbol.h"
#include "type_misc.h"
#include "type_datatype.h"
#include "type_series.h"
#include "type_refinement.h"
#include "type_block.h"
#include "type_anyfunc.h"
#include "type_char.h"
#include "type_word.h"
#include "type_path.h"
#include "type_paren.h"
#include "type_context.h"
#include "type_integer.h"
#include "type_string.h"
#include "type_native.h"
#include "type_function.h"
#include "type_error.h"
#include "core_debug.h"
#include "core_tokenizer.h"
#include "core_eval.h"
#include "type_op.h"
#include "type_action.h"
#include "refinement_defines.h"
#include "core_console.h"

struct type_methods {
    VNATIVE    NativeMakeType;
    char    *(*Mold)(VALUE, UBYTE *);
    char    *(*Form)(VALUE, UBYTE *);
    VALUE    (*Add)(VALUE *);
    void    (*Free)(VALUE);
    VALUE    (*Subtract)(VALUE *);
    VALUE    (*Multiply)(VALUE *);
    VALUE    (*Divide)(VALUE *);
    VALUE    (*Pick)(VALUE, long);
    char    (*Compare)(VALUE, VALUE);
    VALUE    (*PathEval)(VALUE, VALUE);
    VALUE    (*SetPath)(VALUE, VALUE, VALUE);
};

/* === External definitions === */

extern VCONTEXT global_ctx;
extern VTASK    main_task;
extern struct    symtable symbols;

extern struct type_methods *methods[];

extern struct type_methods unset_methods;
extern struct type_methods none_methods;
extern struct type_methods logic_methods;
extern struct type_methods block_methods;
extern struct type_methods word_methods;
extern struct type_methods integer_methods;
extern struct type_methods string_methods;
extern struct type_methods path_methods;
extern struct type_methods refinement_methods;
extern struct type_methods datatype_methods;
extern struct type_methods function_methods;
extern struct type_methods native_methods;
extern struct type_methods action_methods;
extern struct type_methods op_methods;
extern struct type_methods context_methods;
extern struct type_methods error_methods;
extern struct type_methods paren_methods;
extern struct type_methods char_methods;
extern struct type_methods error_methods;

#endif
