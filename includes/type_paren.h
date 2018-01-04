#ifndef _PAREN_H
#define _PAREN_H

/* === Type definition === */

struct VALUE_PAREN {
    HEADER32
    ULONG    slots;            /* Number of available places */
    ULONG    head;            /* Index of first VALUE */
    ULONG    tail;            /* Index of last VALUE */
    void    **data;            /* Array of pointers on VALUEs */
};

typedef struct VALUE_PAREN *VPAREN;

VALUE    NewParen(int);
void    FreeParen(VALUE);
int        AppendParen(VPAREN, VALUE);
VALUE    PickParen(VALUE, long);
char    *MoldParen(VALUE, UBYTE *);

#endif
