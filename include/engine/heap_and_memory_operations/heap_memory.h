#ifndef GUARD_HEAP_MEMORY_H
#define GUARD_HEAP_MEMORY_H
#include "engine/shared/heap.h"
#include "engine/interrupts_and_video/interrupts_video.h"
void Heap_Initialize(void);
void *Heap_Alloc(s32 bytes);
void *Heap_AllocFromEnd(s32 bytes);
void Heap_Free(void *data);
void Memory_Clear(void *data, s32 bytes);

/* Fill halfwords from the end of a GBA RAM range toward its beginning. */
static inline void Memory_Fill16Reverse(u16 *begin, s32 count, u16 value)
{
    s32 cursor;
    if (count <= 0)
        return;
    cursor = (s32)(begin + count - 1);
    do {
        *(u16 *)cursor = value;
        cursor -= (s32)sizeof(*begin);
    } while (cursor >= (s32)begin);
}

#endif
