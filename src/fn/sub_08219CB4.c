#include "global.h"
struct Block { struct Block *prev, *next; u32 size, unused; };
extern struct Block gUnk_02000714;
void *sub_08219CB4(s32 bytes)
{
    struct Block *block = &gUnk_02000714;
    bytes += 15;
    bytes &= ~15;
    bytes += 16;
    { struct Block *next; while ((next = block->next)) block = next; }
    while (block) {
        u32 flags = block->size;

        if ((flags & 0x80000000) && (s32)(flags & 0xFFFFF) >= bytes) {
            s32 size = flags & 0xFFFFF;
            u32 remainder = size - bytes;
            if (remainder > 16) {
                struct Block *next = (struct Block *)((u32)block + size);
                next = (struct Block *)((u32)next - bytes);
                next->size = bytes;
                next->prev = block;
                next->next = block->next;
                if (next->next) next->next->prev = next;
                block->size = remainder | 0x80000000;
                block->next = next;
                block = next;
            } else block->size = flags & 0x7FFFFFFF;
            block->unused = 0;
            return (u8 *)block + 16;
        }
        block = block->prev;
    }
    return 0;
}
