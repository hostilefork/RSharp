#ifndef _MISC_H
#define _MISC_H


struct VALUE_NONE {
    HEADER32
} none_struct_value;

typedef struct VALUE_NONE *VNONE;

VNONE    none_value;

struct VALUE_UNSET {
    HEADER32
} unset_struct_value;

typedef struct VALUE_UNSET *VUNSET;

VUNSET    unset_value;

struct VALUE_LOGIC {
    HEADER32
    UBYTE    data;            /* 0 => TRUE, 1 => FALSE */
};

struct VALUE_LOGIC true_logic_value;
struct VALUE_LOGIC false_logic_value;

typedef struct VALUE_LOGIC *VLOGIC;

VLOGIC    true_value;
VLOGIC    false_value;


char    *MoldNone(VALUE, UBYTE *);
char    CompareNone(VALUE, VALUE);
char    *MoldLogic(VALUE, UBYTE *);
char    CompareLogic(VALUE, VALUE);
char    *MoldUnset(VALUE, UBYTE *);
char    CompareUnset(VALUE, VALUE);

VALUE    RS_Not(VALUE *, VALUE *);
VALUE    RS_Quit(VALUE *, VALUE *);

UBYTE strcmplower(char *, char *);

#endif 

