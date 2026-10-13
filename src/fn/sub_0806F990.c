#include "global.h"
void Memory_Clear(u8 *, u32);
extern u8 *gUnk_02000710;
void sub_0806F990(void)
{
    Memory_Clear(gUnk_02000710 + 0x670, 0x80);
}
