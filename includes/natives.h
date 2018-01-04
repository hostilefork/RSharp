
struct native_def {
    FUNCPTR    body;
    char    *name;
    char    *specs;
};

struct action_def {
    ACTIONPTR    body;
    char        *name;
    char        *specs;
}; 

struct native_def natives[] = {
    {RS_Quit,            "q",                "\n\"Stops evaluation and exits the interpreter.\"\n"},
    {RS_Get,            "get",                "\n\"Gets the value of a word.\"\nword [any-word!] \"Word to get\"\n/any \"Allows any type of value, even unset.\"\n"},
    {RS_Set,            "set",                "\n\"Sets a word or block of words to specified value(s).\"\nword [any-word! block!] \"Word or words to set\"value [any-type!] \"Value or block of values\"/any \"Allows setting words to any value.\"\n"},
    {RS_Valueq,            "value?",            "\n\"Returns TRUE if the word is set.\"value\n"},
    {RS_Unset,            "unset",            "\n\"Unsets the value of a word.\"\nword [word! block!] \"Word or block of words\"\n"},
    {RS_Errorq,            "error?",            "\n\"Returns TRUE for error values.\" value [any-type!]\n"},
    {RS_Make,            "make",                "\n\"Constructs and returns a new value.\"\ntype [any-type!] \"The datatype or example value.\"\n"},
    {RS_Mold,            "mold",                "\n\"Converts a value to a REBOL-readable string.\"\nvalue\"The value to mold\"\n"},
    {RS_Form,            "form",                "\n\"Converts a value to a string.\"\nvalue \"The value to form\"\n"},
    {RS_Copy,            "copy",                "\n\"Returns a copy of a value.\"\nvalue [series! port! bitset!] \"Usually a series\"\n/part \"Limits to a given length or position.\"\nrange [series! integer!]\n/deep \"Also copies series values within the block.\"\n"},
    {RS_Head,            "head",                "\n\"Returns the series at its head.\"\nseries [series! port!]\n"},
    {RS_Headq,            "head?",            "\n\"Returns TRUE if a series is at its head.\"\nseries [series! port! bitset!]\n"},
    {RS_Tail,            "tail",                "\n\"Returns the series at the position after the last value.\"\nseries [series! port!]\n"},
    {RS_Tailq,            "tail?",            "\n\"Returns TRUE if a series is at its tail.\"\nseries [series! port! bitset!]\n"},
    {RS_Next,            "next",                "\n\"Returns the series at its next position.\"\nseries [series! port!]\n"},
    {RS_Back,            "back",                "\n\"Returns the series at its previous position.\"\nseries [series! port!]\n"},
    {RS_Skip,            "skip",                "\n\"Returns the series forward or backward from the current position.\"\nseries [series! port!]\noffset [number! logic!] \"Can be positive, negative, or zero.\"\n"},
    {RS_At,                "at",                "\n\"Returns the series at the specified index.\"\nseries [series!]\nindex [number! logic!] \"Can be positive, negative, or zero.\"\n"},
    {RS_Lengthq,        "length?",            "\n\"Returns the length of the series from the current position.\"\nseries [series! port! tuple! bitset!]\n"},
    {RS_Indexq,            "index?",            "\n\"Returns the index number of the current position in the series.\"series [series! port!]\n"},
    {RS_Typeq,            "type?",            "\n\"Returns a value's datatype.\"\nvalue [any-type!]\n/word \"Returns the datatype as a word.\"\n"},
    {RS_Insert,            "insert",            "\n\"Inserts a value into a series and returns the series after the insert.\"\nseries [series! port! bitset!] \"Series at point to insert\"\nvalue [any-type!] \"The value to insert\"\n/part \"Limits to a given length or position.\"\nrange [number! series! port!]\n/only \"Inserts a series as a series.\"\n/dup \"Duplicates the insert a specified number of times.\"\ncount [number!]\n"},
    {RS_Add,            "add",                "\n\"Returns the result of adding two values.\"\nvalue1 [number! char!]\nvalue2 [number! char!]\n"},
    {RS_Subtract,        "subtract",            "\n\"Returns the second value subtracted from the first.\"\nvalue1 [number! char!]\nvalue2 [number! char!]\n"},
    {RS_Multiply,        "multiply",            "\n\"Returns the first value multiplied by the second.\"\nvalue1 [number! char!]\nvalue2 [number! char!]\n"},
    {RS_Divide,            "divide",            "\n\"Returns the first value divided by the second.\"\nvalue1 [number! char!]\nvalue2 [number! char!]\n"},
    {RS_Output,            "output",            "\n\"Output a string in the console\"\ndata [string!]\n/line \"add a newline\"\n"},
    {RS_Pick,            "pick",                "\n\"Returns the value at the specified position in a series.\"\nseries [series! port! any-function!]\nindex [number! logic!]\n"},
    {RS_Poke,            "poke",                "\n\"Returns value after changing its data at the given index. (See manual)\"\nvalue [series! port!]\nindex [number! logic!]\ndata \"new value\"\n"},
    {RS_Equalq,            "equal?",            "\n\"Returns TRUE if the values are equal.\"value1 value2\n"},
    {RS_NotEqualq,        "not-equal?",        "\n\"Returns TRUE if the values are different.\"value1 value2\n"},
    {RS_StrictEqualq,    "strict-equal?",    "\n\"Returns TRUE if the values are equal and of same type.\"value1 value2\n"},
    {RS_Sameq,            "same?",            "\n\"Returns TRUE if the values are identical.\"value1 value2\n"},
    {RS_Lesserq,        "lesser?",            "\n\"Returns TRUE if first value is lesser than second value.\"value1 value2\n"},
    {RS_Greaterq,        "greater?",            "\n\"Returns TRUE if first value is greater than second value.\"value1 value2\n"},
    {RS_LesserOrEqualq,    "lesser-or-equal?",    "\n\"Returns TRUE if first value is lesser or equal than second value.\"value1 value2\n"},
    {RS_GreaterOrEqualq,"greater-or-equal?","\n\"Returns TRUE if first value is greater or equal than second value.\"value1 value2\n"},
    {RS_Zeroq,            "zero?",            "\n\"Returns TRUE if the number is zero\"number [number! char!]\n"},
    {RS_Not,            "not",                "\n\"Returns the logic complement.\"\nvalue \"(Only FALSE and NONE return TRUE)\"\n"},
    {RS_Stats,            "stats",            "\n\"Show some memory stats\"\n"},
    {RS_Recycle,        "recycle",            "\n\"Recycles unused memory\"\n"},
/* vn */
    {RS_Lowercase,        "lowercase",        "\n\"Converts string of characters to lowercase.\"\nstring [any-string!] \n/part \"Limits to a given length or position.\"\nrange [integer! any-string!]"},
    {RS_Uppercase,        "uppercase",        "\n\"Converts string of characters to uppercase.\"\nstring [any-string!] \n/part \"Limits to a given length or position.\"\nrange [integer! any-string!]"}
/* /vn */
};

struct action_def actions[] = {
    {RS_Do,                "do",                "\n\"Evaluates a block, file, URL, function, word, or any other value.\"\nvalue \"Normally a file name, URL, or block\"\n/args \"If value is a script, this will set its system/script/args\"\narg \"Args passed to a script. Normally a string.\"\n/next \"Do next expression only.  Return block with result and new position.\"\n"},
    {RS_Try,            "try",                "\n\"Tries to DO a block and returns its value or an error.\"block [block!]\n"},
    {RS_Reduce,            "reduce",            "\n\"Evaluates an expression or block expressions and returns the result.\"\nvalue\n"},
    {RS_If,                "if",                "\n\"If condition is TRUE, evaluates the block.\"\ncondition then [block!]\n"},
    {RS_Either,            "either",            "\n\"If condition is TRUE, evaluates the first block, else evaluates the second.\"\ncondition\ntrue-block [block!]\nfalse-block [block!]\n"},
    {RS_Loop,            "loop",                "\n\"Evaluates a block a specified number of times.\"\ncount [integer!] \"Number of repetitions\"\nblock [block!] \"Block to evaluate\"\n"},
    {RS_Until,            "until",            "\n\"Evaluates a block until it is TRUE.\"\nblock [block!]\n"},
    {RS_While,            "while",            "\n\"While a condition block is TRUE, evaluates another block.\"\ncond-block [block!]\nbody-block [block!]\n"},
    {RS_Exit,            "exit",                "\n\"Exits a function, returning no value.\"\n"},
    {RS_Return,            "return",            "\n\"Returns a value from a function.\"nvalue [any-type!]\n"},
    {RS_Break,            "break",            "\n\"Breaks out of a loop\"\n/return \"Forces the loop function to return a value.\"\nvalue [any-type!]\n"},
    {RS_Catch,            "catch",            "\n\"Catches a throw from a block and returns its value.\"\nblock [block!] \"Block to evaluate\"\n/name \"Catches a named throw.\"\nword [word! block!] \"One or more names\"\n"},
    {RS_Throw,            "throw",            "\n\"Throws control back to a previous catch.\"\nvalue [any-type!] \"Value returned from catch\"\n/name \"Throws to a named catch.\"\nword [word!]\n"},
    {RS_Any,            "any",                "\n\"Shortcut OR. Evaluates and returns the first value that is not FALSE or NONE.\"block [block!] \"Block of expressions\"\n"},
    {RS_All,            "all",                "\n\"Shortcut AND. Evaluates and returns at the first FALSE or NONE.\"block [block!] \"Block of expressions\"\n"},
};

