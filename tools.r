REBOL [
    Purpose: "Toolbox for R#"
]

delimiters: {[](){}"; ^@^-^/^M}


build-charset: func [
    "Produce a C representation of a charset"
    str [string!]
    /local out byte cs lc
][
    out: make string! 64
    
    cs: remove/part mold to-binary charset str 3
    remove/part back back tail cs 2
    
    lc: 1
    forall cs  [
        append out rejoin ["0x" copy/part cs 2 ","]
        cs: next cs
        if tail? next cs [remove back tail out]
        if (lc // 8) = 0 [?? lc append out newline]
        lc: lc + 1
    ]
    write clipboard:// out
    print out
]