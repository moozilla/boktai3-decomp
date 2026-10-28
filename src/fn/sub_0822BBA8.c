#include "global.h"
#include "runtime.h"
u32 Save_AlignSizeToEightBytes(u32);
s32 Save_ReadBlockWithRetries(u32, void *, u32);
void Memory_Clear(void *, u32);
u32 Save_ReadHeader(void)
{
    u32 r;
    u32 n = Save_AlignSizeToEightBytes(0x28);
    if (Save_ReadBlockWithRetries(0, gSessionState, n) >= 0)
        r = 1;
    else {
        Memory_Clear(gSessionState, 0x28);
        r = 0;
    }
    return r;
}
