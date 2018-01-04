#ifndef _SERIES_H
#define _SERIES_H


struct VALUE_SERIES {
    HEADER32
    ULONG    slots;        /* Number of available places */
    ULONG    head;        /* Index of first VALUE */
    ULONG    tail;        /* Index of last VALUE */
};

typedef struct VALUE_SERIES *VSERIES;

VALUE    RS_Head(VALUE *, VALUE *);
VALUE    RS_Headq(VALUE *, VALUE *);
VALUE    RS_Tail(VALUE *, VALUE *);
VALUE    RS_Tailq(VALUE *, VALUE *);
VALUE    RS_Next(VALUE *, VALUE *);
VALUE    RS_Back(VALUE *, VALUE *);
VALUE    RS_Skip(VALUE *, VALUE *);
VALUE    RS_At(VALUE *, VALUE *);
VALUE    RS_Lengthq(VALUE *, VALUE *);
VALUE    RS_Indexq(VALUE *, VALUE *);
VALUE    RS_Insert(VALUE *, VALUE *);
VALUE    RS_Pick(VALUE *, VALUE *);
VALUE    RS_Poke(VALUE *, VALUE *);
VALUE    RS_Copy(VALUE *, VALUE *);

#endif
