#ifndef _DATATYPE_H
#define _DATATYPE_H

struct VALUE_DATATYPE {
    HEADER32
    UBYTE    data;            /* TYPE_* */
};

typedef struct VALUE_DATATYPE *VDATATYPE;

struct type_def {
    char        *name;
    VDATATYPE    ref;
} *datatypes;

void    InitDatatypes(void);
char    *GetTypeName(VALUE);
void    *isDatatypeReady();
char    CompareDatatype(VALUE, VALUE);
char    *MoldDatatype(VALUE, UBYTE *);
char    *FormDatatype(VALUE, UBYTE *);

VALUE    RS_Typeq(VALUE *, VALUE *);
VALUE    RS_Make(VALUE *, VALUE *);

#endif
