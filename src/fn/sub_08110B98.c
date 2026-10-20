#include "global.h"

void TileRows_CopyPackedRows(u16 *dest, s32 page, s32 offset, s32 firstRow, s32 width, s32 height,
                             const u16 *source)
{
    s32 row;
    u32 count;
    u16 *dst;
    const u16 *src;
    for (row = 0; row < height; row++) {
        if (width > 0) {
            s32 index = (row + firstRow) * 4 + (offset + page * 80);
            count = row * 64 + (u32)dest;
            dst = (u16 *)count;
            count = width;
            src = (const u16 *)(index * 2 + (u32)source);
            do {
                *dst++ = *src++;
            } while (--count != 0);
        }
    }
}
