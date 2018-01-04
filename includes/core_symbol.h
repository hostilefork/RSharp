#ifndef _SYMBOL_H
#define _SYMBOL_H

struct symtable {
    char    **list;
    ULONG   slots;
    ULONG   tail;
} symbols;

void    InitSymbolsTable(void);
void    FreeSymbolsTable(void);
char    CompareSymbols(ULONG, ULONG);
long    FindSymbol(char *, int);
ULONG    AddSymbol(char *, int);
char    *GetSymbol(ULONG);

char    strncmpi(char *, char *, ULONG);

void DumpSymbols();

#endif
