#include "global.h"
struct Counters { u32 f0, f4; u8 f8, f9, fa; };
extern struct Counters *gUnk_030053F8;
extern u8 *gUnk_02000710;
u8 sub_0822BBA8(void);
void sub_08219DD8(void *, s32);
s32 sub_08225448(void)
{
    u8 result = sub_0822BBA8();
    if (!result) {
        if (gUnk_030053F8) sub_08219DD8(gUnk_030053F8, 40);
        { u32 value = 5; *(u16 *)(gUnk_02000710 + 0x12) = value; }
        { u32 value = 230; gUnk_030053F8->f4 = value; }
        gUnk_030053F8->f8 = result;
        gUnk_030053F8->f9 = result;
        gUnk_030053F8->fa = result;
    }
    return result;
}
