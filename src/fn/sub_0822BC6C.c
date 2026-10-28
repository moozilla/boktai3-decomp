#include "global.h"
#include "ram.h"
extern u32 *gGameState;
extern u32 gUnk_03005404;
u32 Save_AlignSizeToEightBytes(u32);
u32 Save_GetPrimaryRegionBlock(u32, u32), Save_GetMainStateSize(void);
s32 Save_ReadBlockWithRetries(u32, void *, u32);
u32 Save_ReadMainState(u32 arg)
{
    u32 size = Save_AlignSizeToEightBytes(1024);
    if (Save_ReadBlockWithRetries((u16)Save_GetPrimaryRegionBlock(arg, 0), gGameVariables, size) <
        0)
        return 0;
    size = Save_AlignSizeToEightBytes(Save_GetMainStateSize());
    if (Save_ReadBlockWithRetries((u16)Save_GetPrimaryRegionBlock(arg, 1), gGameState, size) < 0)
        return 0;
    if (*gGameState != gSaveStateSignature)
        return 0;
    return gUnk_03005404 = 1;
}
