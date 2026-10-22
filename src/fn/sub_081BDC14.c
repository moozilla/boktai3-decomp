#include "global.h"
struct Bg {
    u8 pad[0x2c];
    u16 *map;
    u8 pad2[0];
};
extern struct Bg gUnk_03004C30[];
u16 *sub_081BDC14(u32 bg, u32 x, u32 y)
{
    struct Bg *b = &gUnk_03004C30[bg];
    u16 *m = b->map;
    return m + (x & 31) + (y & 31) * 32;
}