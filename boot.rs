;======================
;     R# definitions
;======================

yes: on: true
no: off: false
empty?: :tail?
quit: :q

tab: #"^-"
cr: #"^M"
newline: lf: #"^/"
escape: #"^["
slash: #"/"

+:  make op! :add
-:  make op! :subtract
*:  make op! :multiply
/:  make op! :divide
=:  make op! :equal?
<>: make op! :not-equal?
==: make op! :strict-equal?
=?: make op! :same?
<:  make op! :lesser?
>:  make op! :greater?
<=: make op! :lesser-or-equal?
>=: make op! :greater-or-equal?

!=: :<>        ; to make C fans happy ;-)

func: make function! [spec body][
    make function! spec body
]

does: func [body [block!]][make function! [] body]

append: func [serie [series!] value][
    head insert tail serie value
]

repend: func [serie [series!] value][
    head insert tail serie reduce :value
]

print: func [value][output/line form reduce :value]

prin: func [value][output form reduce :value]

probe: func [value][output/line mold :value :value]

attempt: func [value][
    if not error? set/any 'value try :value [get/any 'value]
]

forever: func [
    "Endless block evaluation"
    body [block!] "Block to evaluate each time"
][
    while [true] body
]

integer?:     func [value [any-type!]][integer!    = type? value]
block?:         func [value [any-type!]][block!    = type? value]
char?:         func [value [any-type!]][char!!    = type? value]
datatype?:     func [value [any-type!]][datatype!    = type? value]
function?:     func [value [any-type!]][function!    = type? value]
none?:         func [value [any-type!]][none!        = type? value]
logic?:         func [value [any-type!]][logic!    = type? value]
unset?:         func [value [any-type!]][unset!    = type? value]
native?:     func [value [any-type!]][native!    = type? value]
op?:         func [value [any-type!]][op!        = type? value]
paren?:         func [value [any-type!]][paren!    = type? value]
path?:         func [value [any-type!]][path!        = type? value]
refinement?: func [value [any-type!]][refinement! = type? value]
string?:     func [value [any-type!]][string!    = type? value]
word?:         func [value [any-type!]][word!        = type? value]


; --- temporary definitions ---
first:    func [s [series!]][pick s 1]
second: func [s [series!]][pick s 2]
third:    func [s [series!]][pick s 3]
fourth:    func [s [series!]][pick s 4]
fifth:    func [s [series!]][pick s 5]

last: func [value [series!]][pick value length? value]

; --- for testing purpose only ---
align: func [str [string!] n [integer!] /with c [char!]][
    head insert/dup str any [all [with c] #" "] n - length? str
]

;;; recursivity not yet supported
;;; !: func [a] [either a > 1 [a * ! (a - 1)][1]]
