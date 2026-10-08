#include "global.h"
extern u8 *gUnk_02000710;
u32 sub_0803F838(u32 k, s32 n)
{
    u32 sh = n * 3 * 2;
    u32 m = 0x3f;
    u32 r;
    s32 v;
    switch (k) {
    case 0: v = *(u32 *)(gUnk_02000710 + 0x7b8); r = (v >> sh) & m; break;
    case 1: v = *(u32 *)(gUnk_02000710 + 0x7c0); r = (v >> sh) & m; break;
    case 2: v = *(u32 *)(gUnk_02000710 + 0x7bc); r = (v >> sh) & m; break;
    case 3: v = *(u32 *)(gUnk_02000710 + 0x7c4); r = (v >> sh) & m; break;
    case 4: v = *(u32 *)(gUnk_02000710 + 0x81c); r = (v >> sh) & m; break;
    case 5: v = *(s16 *)(gUnk_02000710 + 0x7f8); r = (v >> sh) & m; break;
    default: r = 0; break;
    }
    return r;
}
