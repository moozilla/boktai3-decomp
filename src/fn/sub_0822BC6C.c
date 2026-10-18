#include "global.h"
extern void *gUnk_02000708;
extern u32 *gUnk_02000710;
extern u32 gUnk_0200060C, gUnk_03005404;
u32 Save_AlignSizeToEightBytes(u32);
u32 Save_GetPrimaryRegionBlock(u32, u32), Save_GetMainStateSize(void);
s32 sub_0822F10C(u32, void *, u32);
u32 Save_ReadMainState(u32 arg)
{
    u32 size = Save_AlignSizeToEightBytes(1024);
    if (sub_0822F10C((u16)Save_GetPrimaryRegionBlock(arg, 0), gUnk_02000708, size) < 0)
        return 0;
    size = Save_AlignSizeToEightBytes(Save_GetMainStateSize());
    if (sub_0822F10C((u16)Save_GetPrimaryRegionBlock(arg, 1), gUnk_02000710, size) < 0)
        return 0;
    if (*gUnk_02000710 != gUnk_0200060C)
        return 0;
    return gUnk_03005404 = 1;
}
