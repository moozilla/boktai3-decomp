#include "global.h"
struct G { u8 f[0x77C]; s32 v; };
extern struct G *gUnk_02000710;
u16 *sub_08163F70(s32, s32, s32);
void sub_08165488(s32, s32, s32, s32, s32);
void sub_0816568C(void)
{
    u16 *p = sub_08163F70(0, 0x18, 1);
    *p = 0xF001;
    if (gUnk_02000710->v > 9999)
        gUnk_02000710->v = 9999;
    sub_08165488(gUnk_02000710->v, 1000, 0x19, 1, 0);
}
