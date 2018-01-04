#ifndef _GC_H
#define _GC_H

#include "rsharp.h"

#define MB_TYPE_CTX            1
#define MB_TYPE_ARRAY        2

#define DEBUG_MEM    0
#define MEM_TEST    0

/*--- VALUES definitions ---*/

#define MAX_VALUES            5000                        /* max values per frame  (multiple of 2) */
#define FRM_VALUE_SIZE        (4 + max_value_size) * MAX_VALUES + struct_frame_value_size + 8
#define SFRMV                struct frame_value

struct frame_value;
struct frame_value {
    SFRMV    *next;            /* link to next frame or NULL to end the list */
    void    *base;            /* points to start of allocatable space */
    void    *stack;            /* points to stack bottom */
    void    **top;            /* points to current stack top */
};

ULONG struct_frame_value_size;
ULONG max_value_size;        /* Max size in bytes of a value */

/*--- Raw Memory definitions ---*/

#define MBH                struct block_header
#define MBH_SIZE        sizeof(struct block_header)
#define SBH(x)            ((MBH *)x)
#define GET_MB_DATA(x)    (void *)((char *)x + sizeof(struct block_header))    
#define GET_MB_HEAD(x)    (MBH *)((char *)x - sizeof(struct block_header))

struct block_header;
struct block_header {
    MBH        *next;
    MBH        *prev;
    ULONG    size;
    ULONG    mark : 1;
    ULONG    save : 1;
    ULONG    reserved : 30;
};

/*--- Main Memory Structure ---*/

struct memory {
    struct {
        SFRMV    *head;        /* root VALUEs memory frame */
        SFRMV    *active;    /* active memory frame */
    } values;
    struct {
        MBH        *head;        /* start node of block list */
        MBH        *tail;        /* end node of block list */
    } data;
} mem;


void    *InitMem(void);
void    *AllocValue(void);
void    *CloneValue(VALUE);
void    *AllocMem(ULONG);
void    *AllocMemForever(ULONG);
void    *ReAllocMem(void *, ULONG);
void    CheckGC();
void    GarbageCollect();
void    FreeMem(void *);
void    FreeAllMem();

void    MemTest();                            /* debugging only */

VALUE    RS_Stats(VALUE *, VALUE *);
VALUE    RS_Recycle(VALUE *, VALUE *);

#endif
