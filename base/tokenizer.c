#include "rsharp.h"

#define IS_DELIMITER(i)        charset_delimiters[i>>3] & 1<<(i%8)
#define IS_DIGIT(v)            ('0' <= v) && (v <= '9')
#define SYN_ERROR(x)        NewError(0, x, blk_stack[top])
#define STACK_MAX_SIZE        256

#define ST_START        0
#define ST_BLOCK        1
#define ST_STRING        3
#define ST_PLUSMINUS    10
#define ST_INT            11
#define ST_GETWORD        19
#define ST_GETWORDE        20
#define ST_WORD            21
#define ST_SETWORD        22
#define ST_PATHBEG        23
#define ST_PATHE        24
#define ST_PATHNUM        25
#define ST_PATHGETW        26
#define ST_SETPATH        27
#define ST_LITMODE        28
#define ST_REFINBEG        29
#define ST_REFINEW        30
#define ST_COMMENT        31
#define ST_SHARP        32
#define ST_CHARBEG        33
#define ST_CHARANY        34
#define ST_CHARCLOSE    35
#define ST_CHAREND        36


/* Blanks: null space tab cr lf */
UBYTE charset_blanks[32] = {
    0x01,0x26,0x00,0x00,0x01,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
/* Delimiters: [](){}"; null space tab cr lf */
UBYTE charset_delimiters[32] = {
    0x01,0x26,0x00,0x00,0x05,0x03,0x00,0x08,
    0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x28,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
/* Strict Word: a-z A-Z ?!.*&|=_~ and all ascii >= 127  */
UBYTE charset_sword[32] = {
    0x00,0x00,0x00,0x00,0x42,0x44,0x00,0xF0,
    0xFE,0xFF,0xFF,0x97,0xFF,0xFF,0xFF,0x57,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};
/* Word: 0-9 a-z A-Z ?!.'+-*&|=_~ and all ascii >= 127 */
UBYTE charset_word[32] = {
    0x00,0x00,0x00,0x00,0xC2,0x6C,0xFF,0xF3,
    0xFE,0xFF,0xFF,0x97,0xFF,0xFF,0xFF,0x57,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

VBLOCK blk_stack[STACK_MAX_SIZE];

VALUE Tokenize(char *src, char *end)
{
    VPATH    path;
    VALUE    val;
    char    type;
    int        top = 0, get_mode = 0, lit_mode = 0;
    int        state = ST_START;
    char    *tok_start, close_type;

    blk_stack[0] = NewBlock(0);
    close_type = ']';                        /* just needed for distincton between paren! and block! errors */

    if ((strlen(src) == 0) || (src == end))
        return blk_stack[0];

    path = NULL;

    tok_start = src--;                        /* simplify detection of end of string */
    for(;;)
    {
        src++;
        switch(state)
        {
            case ST_START:

                if (IN_BITSET(charset_blanks, *src))
                    break;

                tok_start = src;
                lit_mode = 0;

                if (IS_DIGIT(*src))    /* 0..9 */
                {
                    state = ST_INT;
                    break;
                }
                if (IN_BITSET(charset_sword,*src))
                {
                    state = ST_WORD;
                    break;
                }
                switch(*src)
                {
                    case '+': state = ST_PLUSMINUS; break;
                    case '-': state = ST_PLUSMINUS; break;
                    case ':': state = ST_GETWORD;    break;
                    case '"': state = ST_STRING;    break;
                    case '/': state = ST_REFINBEG;    break;
                    case ';': state = ST_COMMENT;    break;
                    case '#': state = ST_SHARP;        break;
                    case '\'': state = ST_LITMODE;    break;
                    case '[':
                        _ASSERT(top < STACK_MAX_SIZE);
                        blk_stack[++top] = NewBlock(0);
                        break;
                    case ']':
                        top--;
                        close_type = ']';
                        if (top >= 0)
                            AppendBlock(blk_stack[top], blk_stack[top + 1]);
                        break;
                    case '(':
                        _ASSERT(top < STACK_MAX_SIZE);
                        blk_stack[++top] = NewParen(0);
                        break;
                    case ')':
                        top--;
                        close_type = ')';
                        if (top >= 0)
                            AppendParen(PAREN(blk_stack[top]), blk_stack[top + 1]);
                        break;
                }
                break;

            case ST_STRING:

                if (*src == '"')
                {
                    val = NewString(tok_start + 1, src - tok_start - 1);
                    AppendBlock(blk_stack[top], val);
                    state = ST_START;
                    break;
                }
                if (*src == '^')
                {
                    /* Escape sequence : TBD */
                    break;
                }
                break;

            case ST_WORD:

                if (IN_BITSET(charset_word, *src))
                    break;
                if (IS_DELIMITER(*src) || (src == end))
                {
                    type = lit_mode ? TYPE_LIT_WORD : TYPE_WORD;
                    val = NewWord(tok_start, src - tok_start, type);
                    AppendBlock(blk_stack[top], val);
                    state = ST_START;
                    src--;
                    break;
                }
                if (*src == ':')
                {
                    state = ST_SETWORD;
                    break;
                }
                if (*src == '/')
                {
                    state = ST_PATHBEG;
                    break;
                }
                return NewError(0, 0, blk_stack[top]);
                break;

            case ST_INT:

                if (IS_DIGIT(*src))
                    break;
                if (IS_DELIMITER(*src) || (src == end))
                {
                    val = NewInteger(tok_start, src - tok_start);
                    AppendBlock(blk_stack[top], val);
                    src--;
                    state = ST_START;
                    break;
                }
                return NewError(0, 1, blk_stack[top]);
                break;

            case ST_PLUSMINUS:

                if (IS_DIGIT(*src))
                {
                    state = ST_INT;
                    break;
                }
                if (IN_BITSET(charset_word, *src))
                {
                    state = ST_WORD;
                    break;
                }
                src--;
                state = ST_WORD;
                break;

            case ST_GETWORD:

                if (IN_BITSET(charset_sword,*src) || (*src == '+') || (*src == '-'))
                {
                    state = ST_GETWORDE;
                    break;
                }
                return NewError(0, 2, blk_stack[top]);
                break;

            case ST_GETWORDE:

                if (IN_BITSET(charset_word, *src))
                    break;
                if (IS_DELIMITER(*src) || (src == end))
                {
                    val = NewWord(tok_start + 1, src - tok_start - 1, TYPE_GET_WORD);
                    AppendBlock(blk_stack[top], val);
                    src--;
                    state = ST_START;
                    break;
                }
                return NewError(0, 2, blk_stack[top]);
                break;

            case ST_SETWORD:

                if (IS_DELIMITER(*src) || (src == end))
                {
                    val = NewWord(tok_start, src - tok_start - 1, TYPE_SET_WORD);
                    AppendBlock(blk_stack[top], val);
                    src--;
                    state = ST_START;
                    break;
                }
                return NewError(0, 3, blk_stack[top]);
                break;

            case ST_PATHBEG:

                if (!path)
                {
                    path = NewPath(0);
                    if (lit_mode)
                        path->type = TYPE_LIT_PATH;

                    AppendBlock(blk_stack[top], path);
                    val = NewWord(tok_start, src - tok_start - 1, TYPE_WORD);
                    AppendPath(path, val);
                }
                tok_start = src;
                get_mode = 0;
                if (*src == ':')
                {
                    get_mode = 1;
                    state = ST_PATHGETW;
                    break;
                }
                if (IN_BITSET(charset_sword,*src))
                {
                    state = ST_PATHE;
                    break;
                }
                if (IS_DIGIT(*src))    /* 0..9 */
                {
                    state = ST_PATHNUM;
                    break;
                }
                return NewError(0, 9, blk_stack[top]);
                break;

            case ST_PATHE:

                if (IS_DELIMITER(*src) || (src == end))
                {
                    type = get_mode ? TYPE_GET_WORD : TYPE_WORD;
                    val = NewWord(tok_start, src - tok_start, type);
                    AppendPath(path, val);
                    state = ST_START;
                    path = NULL;
                    src--;
                    break;
                }
                if (IN_BITSET(charset_word, *src))
                {
                    state = ST_PATHE;
                    break;
                }
                if ((*src == '/') || (*src == ':'))
                {
                    type = get_mode ? TYPE_GET_WORD : TYPE_WORD;
                    val = NewWord(tok_start, src - tok_start, type);
                    AppendPath(path, val);
                    state = (*src == '/') ? ST_PATHBEG : ST_SETPATH;        /* à vérifier ! */
                    break;
                }
                return NewError(0, 9, blk_stack[top]);
                break;

            case ST_PATHNUM:

                if (IS_DELIMITER(*src) || (src == end))
                {
                    val = NewInteger(tok_start, src - tok_start);
                    AppendPath(path, val);
                    state = ST_START;
                    path = NULL;
                    src--;
                    break;
                }
                if (IS_DIGIT(*src))    /* 0..9 */
                {
                    state = ST_PATHNUM;
                    break;
                }
                if ((*src == '/') || (*src == ':'))
                {
                    val = NewInteger(tok_start, src - tok_start);
                    AppendPath(path, val);
                    state = (*src == '/') ? ST_PATHBEG : ST_SETPATH;
                    break;
                }
                return NewError(0, 9, blk_stack[top]);
                break;

            case ST_PATHGETW:

                if (IN_BITSET(charset_sword,*src))
                {
                    tok_start = src;
                    state = ST_PATHE;
                    break;
                }
                return NewError(0, 9, blk_stack[top]);
                break;

            case ST_SETPATH:
        
                path->type = TYPE_SET_PATH;
                path = NULL;
                state = ST_START;
                break;

            case ST_LITMODE:
        
                lit_mode = 1;
                if (IN_BITSET(charset_sword,*src) || (*src == '+') || (*src == '-'))
                {
                    tok_start++;
                    state = ST_WORD;
                    break;
                }
                return NewError(0, 17, blk_stack[top]);
                break;

            case ST_REFINBEG:

                if (IN_BITSET(charset_sword,*src))
                {
                    state = ST_REFINEW;
                    break;
                }
                if (*src == ':')
                {
                    state = ST_SETWORD;
                    break;
                }
                if (IS_DELIMITER(*src) || (src == end))
                {
                    val = NewWord(tok_start, src - tok_start, TYPE_WORD);
                    AppendBlock(blk_stack[top], val);
                    state = ST_START;
                    src--;
                    break;
                }
                return NewError(0, 10, blk_stack[top]);
                break;

            case ST_REFINEW:

                if (IS_DELIMITER(*src) || (src == end))
                {
                    tok_start++;
                    val = NewRefinement(tok_start, src - tok_start);
                    AppendBlock(blk_stack[top], val);
                    state = ST_START;
                    src--;
                    break;
                }
                if (IN_BITSET(charset_word,*src))
                {
                    state = ST_REFINEW;
                    break;
                }
                return NewError(0, 10, blk_stack[top]);
                break;

            case ST_COMMENT:

                if ((*src != '\n') || (src == end))
                {
                    state = ST_COMMENT;
                    break;
                }
                state = ST_START;
                break;

            case ST_SHARP:
                
                if (*src == '"')
                {
                    state = ST_CHARBEG;
                    break;
                }
                return NewError(0, 21, blk_stack[top]);
                break;

            
            case ST_CHARBEG:
                
                if (*src == '"')
                {
                    type = -1;
                    state = ST_CHAREND;
                    break;
                }
                if (*src == '^')
                {
                    type = 1;
                    state = ST_CHARANY;
                    break;
                }
                type = 0;
                state = ST_CHARCLOSE;
                break;

            case ST_CHARANY:
                
                state = ST_CHARCLOSE;
                break;

            case ST_CHARCLOSE:

                if (*src == '"')
                {
                    state = ST_CHAREND;
                    break;
                }
                return NewError(0, 21, blk_stack[top]);

            case ST_CHAREND:
            
                val = NewChar(*(tok_start + 2 + type), type);
                AppendBlock(blk_stack[top], val);
                state = ST_START;
                src--;
                break;

        }
         if (!*src || (end && src > end))
            break;
    }

    if (state == ST_STRING)
        return NewError(0, 4, blk_stack[top]);

    if (top > 0)
    {
        if (TYPE(blk_stack[top]) == TYPE_BLOCK)
            return NewError(0, 5, blk_stack[top]);
        else
            return NewError(0, 18, blk_stack[top]);
    }

    if (top < 0)
    {
        if (close_type == ']')
            return NewError(0, 16, NULL);
        else
            return NewError(0, 19, NULL);
    }

    return blk_stack[0];
}

