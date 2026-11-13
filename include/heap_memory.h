#ifndef GUARD_HEAP_MEMORY_H
#define GUARD_HEAP_MEMORY_H
#include "heap.h"
#include "interrupts_video.h"
void Heap_Initialize(void);
void *Heap_Alloc(s32 bytes);
void *Heap_AllocFromEnd(s32 bytes);
void Heap_Free(void *data);
void Memory_Clear(void *data, s32 bytes);
#endif
