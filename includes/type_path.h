#ifndef _PATH_H
#define _PATH_H

/* === Type definition === */

struct VALUE_PATH {
    HEADER32
    ULONG    slots;        /* Number of available places */
    ULONG    head;        /* Index of first VALUE */
    ULONG    tail;        /* Index of last VALUE */
    void    **data;        /* Array of pointers on VALUEs */
};

typedef struct VALUE_PATH *VPATH;

VALUE    NewPath(int);
VALUE    AltNewPath(VPATH);
void    FreePath(VALUE);
int        AppendPath(VPATH, VALUE);
VALUE    PickPath(VALUE, long);
char    *MoldPath(VALUE, UBYTE *);

#endif
