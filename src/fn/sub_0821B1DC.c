#include "global.h"
extern u8 *gUnk_02000710, *gUnk_0200070C, *gUnk_02000708, *gUnk_02000704, *gUnk_02000700;
void sub_08219DD8(u8 *, u32);
void sub_08219EBC(u8 *, u8 *, u32);
u8 *sub_0821B1DC(u8 *p)
{
    s32 d = p - gUnk_02000710;
    return gUnk_0200070C + d;
}
