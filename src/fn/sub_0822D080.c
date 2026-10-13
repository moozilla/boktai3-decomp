#include "global.h"
#ifdef NONMATCHING
// fuzzy: 84.4%
// 3 differing instruction runs; first diff at +0x8 (ldr r1, [r0] vs ldr r0, [r0])
extern u8 *gUnk_02000710;
s32 sub_0822CCC8(s32);
void sub_0822CCE0(s32, u32);
void sub_0822D080(s32 a, s32 b)
{
    u8 *ctx = gUnk_02000710;
    s16 *first = (s16 *)(a * 2 + ctx + 0xa0);
    s32 v = *first;
    s16 *second = (s16 *)(b * 2 + ctx + 0xa0);
    *first = *second;
    *second = v;
    v = sub_0822CCC8(a);
    sub_0822CCE0(a, sub_0822CCC8(b));
    sub_0822CCE0(b, v);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0822D080);
#endif
