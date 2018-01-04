#ifndef _BLOCK_H
#define _BLOCK_H

/* === Type definition === */

struct VALUE_BLOCK {
    HEADER32
    ULONG    slots;        /* Number of available places */
    ULONG    head;        /* Index of first VALUE */
    ULONG    tail;        /* Number of values in this block! */
    void    **data;        /* Array of pointers on VALUEs */
};

typedef struct VALUE_BLOCK *VBLOCK;

VALUE    NewBlock(int);
void    FreeBlock(VALUE);
int        AppendBlock(VBLOCK, VALUE);
VALUE    PathEvalBlock(VALUE, VALUE);
VALUE    SetPathBlock(VALUE, VALUE, VALUE);
VALUE    InsertBlock(VALUE *, VALUE *);
VALUE    CopyBlock(VALUE *, VALUE *);
VALUE    PickBlock(VALUE, long);
VALUE    PokeBlock(VALUE, VALUE, VALUE);
char    CompareBlock(VALUE, VALUE);
char    *MoldBlock(VALUE, UBYTE *);
char    *FormBlock(VALUE, UBYTE *);

#endif
