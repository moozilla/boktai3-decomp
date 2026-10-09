#include "global.h"
struct Block { struct Block *prev, *next; u32 size, unused; };
extern struct Block gUnk_02000714;
void *sub_08219C40(s32 bytes)
{
    struct Block *block = &gUnk_02000714;
    bytes += 15;
    bytes &= ~15;
    bytes += 16;
    while (block) {
        u32 flags = block->size;
        if ((flags & 0x80000000) && (s32)(flags & 0xFFFFF) >= bytes) {
            u32 remainder = (flags & 0xFFFFF) - bytes;
            if (remainder > 16) {
                struct Block *next = (struct Block *)((u8 *)block + bytes);
                next->size = remainder | 0x80000000;
                do {
                    next->prev = block;
                    next->next = block->next;
                    if (next->next) next->next->prev = next;
                    block->size = bytes;
                    block->next = next;
                } while (0);
            } else block->size = flags & 0x7FFFFFFF;
            block->unused = 0;
            return (u8 *)block + 16;
        }
        block = block->next;
    }
    return 0;
}
