#include "global.h"
struct Reference { u8 bytes[4]; u16 first, index; };
extern u8 *gUnk_02000710, *gUnk_02000700, *gUnk_02000708;
void sub_0821B34C(u8 *, u32, s32, s32);
void sub_0821B4C8(struct Reference *ref, s32 index, s32 value)
{
    u32 descriptor = (ref->bytes[0] << 24) | (ref->bytes[1] << 16) | (ref->bytes[2] << 8) | ref->bytes[3];
    u8 *base;
    if ((descriptor & 0xF00000) == 0x800000)
        base = gUnk_02000710;
    else if ((descriptor & 0xF00000) == 0x100000)
        base = gUnk_02000700;
    else
        base = gUnk_02000708;
    base += descriptor & 0xFFFF;
    if (((descriptor >> 24) & 0xF0) == 0x20)
        index += ref->index;
    sub_0821B34C(base, descriptor, index, value);
}
