/* === Type definition === */


struct VALUE_CHAR {
    HEADER32
    UBYTE    data;
};

typedef struct VALUE_CHAR *VCHAR;

VALUE    NewChar(char, char);
void    FreeChar(VALUE p);
char    *MoldChar(VALUE, UBYTE *);
char    CompareChar(VALUE, VALUE);
