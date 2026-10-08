#include "global.h"
struct G { u8 f[0x50C]; s32 v; };
extern struct G *gUnk_02000710;
u16 *sub_08163F70(s32, s32, s32);
void sub_08165488(s32, s32, s32, s32, s32);
void sub_08165634(s32 a, s32 b)
{
    u16 *p = sub_08163F70(0, a - 1, b);
    *p = 0xF001;
    if (gUnk_02000710->v > 9999)
        gUnk_02000710->v = 9999;
    sub_08165488(gUnk_02000710->v, 1000, a, b, 1);
}
