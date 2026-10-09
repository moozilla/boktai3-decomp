#include "global.h"
struct Reference { u8 bytes[4]; u16 first, index; };
extern u8 *gUnk_0200070C, *gUnk_02000704;
u8 *sub_0821B478(u8 *, struct Reference *);
void sub_0821B20C(u8 *, u32, s32, s32 *);
s32 sub_0821B63C(u8 *p)
{
    struct Reference ref;
    u8 *r = (u8 *)&ref;
    s32 value;
    u8 *base;
    s32 index = 0;
    u32 descriptor;
    sub_0821B478(p, &ref);
    descriptor = (r[0] << 24) | (r[1] << 16) | (r[2] << 8) | r[3];
    if ((descriptor & 0xF00000) == 0x800000)
        base = gUnk_0200070C;
    else
        base = gUnk_02000704;
    base += descriptor & 0xFFFF;
    if (((descriptor >> 24) & 0xF0) == 0x20)
        index += *(u16 *)(r + 6);
    sub_0821B20C(base, descriptor, index, &value);
    return value;
}
