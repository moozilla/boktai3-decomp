#include "global.h"
struct Block { struct Block *prev, *next; u32 size, unused; };
void sub_08219D38(void *data)
{
    struct Block *block, *prev, *next;
    u32 flags;
    if (!data) return;
    block = (struct Block *)((u8 *)data - 16);
    prev = block->prev;
    next = block->next;
    flags = block->size;
    if (flags & 0x80000000) return;
    block->size = flags | 0x80000000;
    block->unused = 0;
    if (prev && (prev->size & 0x80000000)) {
        prev->size = ((prev->size & 0xFFFFF) + (block->size & 0xFFFFF)) | 0x80000000;
        prev->next = next;
        if (next) block->next->prev = prev;
        block = prev;
    }
    if (next && (next->size & 0x80000000)) {
        block->size = ((block->size & 0xFFFFF) + (next->size & 0xFFFFF)) | 0x80000000;
        block->next = next->next;
        if (block->next) next->next->prev = block;
    }
}
