#include "global.h"
extern u8 *gUnk_02000710, *gUnk_0200070C, *gUnk_02000708, *gUnk_02000704, *gUnk_02000700;
void Memory_Clear(u8 *, u32);
void Memory_Copy(u8 *, u8 *, u32);
void sub_0821B180(void)
{
    Memory_Copy(gUnk_02000710, gUnk_0200070C, 0x89C);
    Memory_Copy(gUnk_02000708, gUnk_02000704, 0x400);
}
