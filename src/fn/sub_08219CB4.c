#include "global.h"
#include "heap.h"
void *Heap_AllocFromEnd(s32 bytes)
{
    struct HeapBlock *block = &gHeapHead;
    bytes += 15;
    bytes &= ~15;
    bytes += 16;
    {
        struct HeapBlock *next;
        while ((next = block->next))
            block = next;
    }
    while (block) {
        u32 flags = block->size;

        if ((flags & 0x80000000) && (s32)(flags & 0xFFFFF) >= bytes) {
            s32 size = flags & 0xFFFFF;
            u32 remainder = size - bytes;
            if (remainder > 16) {
                struct HeapBlock *next = (struct HeapBlock *)((u32)block + size);
                next = (struct HeapBlock *)((u32)next - bytes);
                next->size = bytes;
                next->prev = block;
                next->next = block->next;
                if (next->next)
                    next->next->prev = next;
                block->size = remainder | 0x80000000;
                block->next = next;
                block = next;
            } else
                block->size = flags & 0x7FFFFFFF;
            block->unused = 0;
            return (u8 *)block + 16;
        }
        block = block->prev;
    }
    return 0;
}
