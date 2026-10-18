#include "global.h"
extern u8 *gUnk_030053F8;
u32 Save_AlignSizeToEightBytes(u32);
s32 Save_ReadBlockWithRetries(u32, void *, u32);
void Memory_Clear(void *, u32);
u32 Save_ReadHeader(void)
{
    u32 r;
    u32 n = Save_AlignSizeToEightBytes(0x28);
    if (Save_ReadBlockWithRetries(0, gUnk_030053F8, n) >= 0)
        r = 1;
    else {
        Memory_Clear(gUnk_030053F8, 0x28);
        r = 0;
    }
    return r;
}
