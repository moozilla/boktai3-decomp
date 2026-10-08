#include "global.h"
extern u8 *gUnk_02000710, *gUnk_0200070C, *gUnk_02000708, *gUnk_02000704, *gUnk_02000700;
void sub_08219DD8(u8 *, u32);
void sub_08219EBC(u8 *, u8 *, u32);
void sub_0821B1B8(u8 *p, u32 n)
{
    s32 d = p - gUnk_02000710;
    sub_08219EBC(gUnk_0200070C + d, p, n);
}
