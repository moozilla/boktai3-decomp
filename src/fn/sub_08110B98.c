
#ifdef NONMATCHING
// fuzzy: 93.8%
// foundry draft (worker best 93.8%); what differs is mostly register allocation and instruction order
#define TileRows_CopyPackedRows sub_08110B98
#include "global.h"
void sub_08110B98(u16 *dest, s32 page, s32 offset, s32 firstRow, s32 width, s32 height,
                  const u16 *source)
{
    s32 row;
    u32 count;
    u16 *dst;
    const u16 *src;
    for (row = 0; row < height; row++) {
        if (width > 0) {
            s32 index = (row + firstRow) * 4 + (offset + page * 80);
            count = width;
            dst = (u16 *)(row * 64 + (u32)dest);
            src = (const u16 *)(index * 2 + (u32)source);
            do {
                *dst++ = *src++;
            } while (--count != 0);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08110B98);
#endif
