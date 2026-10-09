#include "global.h"
extern u8 *gUnk_02000710;
extern u8 *gUnk_02000700;
extern u8 *gUnk_02000708;
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
void sub_0821B34C(u8 *, u32, s32, s32);
u8 *sub_0821B3E4(u8 *p, s32 value)
{
    s32 ignored, first, index;
    u32 descriptor = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    u8 *base, *end;
    if ((descriptor & 0xF00000) == 0x800000)
        base = gUnk_02000710;
    else if ((descriptor & 0xF00000) == 0x100000)
        base = gUnk_02000700;
    else
        base = gUnk_02000708;
    base += descriptor & 0xFFFF;
    if (((descriptor >> 24) & 0xF0) == 0x20) {
        end = Script_DecodeOperand(p + 4, &ignored, &first);
        end = Script_DecodeOperand(end, &ignored, &index);
    } else {
        index = 0;
        end = p + 4;
    }
    sub_0821B34C(base, descriptor, index, value);
    return end;
}
