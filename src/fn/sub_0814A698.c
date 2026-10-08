#include "global.h"

extern u8 *gUnk_02000710;
extern u8 gUnk_08E60F4C[];

s32 sub_0814A698(s32 unused, s32 idx)
{
    u8 *t = gUnk_08E60F4C;
    u8 *e = t + (idx << 2);
    return *(s16 *)(gUnk_02000710 + 0x40) + e[1];
}
