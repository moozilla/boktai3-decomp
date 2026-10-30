#ifndef GUARD_HEAP_H
#define GUARD_HEAP_H

#include "global.h"

/* RAM objects; each row of symbols/ram.csv with header include/heap.h. */

struct HeapBlock {
    struct HeapBlock *prev, *next;
    u32 size; /* Low 20 bits: size including this header; bit 31: free. */
    u32 unused;
};
extern struct HeapBlock gHeapHead;

#define HEAP_HEAD_ADDR 0x02000714 /* gHeapHead */

#endif
