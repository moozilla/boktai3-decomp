#include "global.h"
extern u8 *gUnk_02000710;
extern u32 gUnk_0200060C, gUnk_03005404;
void GameState_BackupRegionRange(void *, u32);
u32 Save_AlignSizeToEightBytes(u32);
u32 Save_GetPrimaryRegionBlock(u32, u32);
void *GameState_GetBackupVariableBase(void), *GameState_GetBackupStateBase(void);
s32 sub_0822F0A4(u32, void *, u32);
u32 Save_GetMainStateSize(void);
u32 Save_WriteMainState(u32 arg)
{
    u32 buffer, result, index;
    *(u32 *)gUnk_02000710 = gUnk_0200060C;
    GameState_BackupRegionRange(gUnk_02000710, 4);
    buffer = Save_AlignSizeToEightBytes(1024);
    index = Save_GetPrimaryRegionBlock(arg, 0);
    index <<= 16;
    index >>= 16;
    result = sub_0822F0A4(index, GameState_GetBackupVariableBase(), buffer);
    if (result != buffer + 8)
        return 0;
    buffer = Save_AlignSizeToEightBytes(Save_GetMainStateSize());
    index = Save_GetPrimaryRegionBlock(arg, 1);
    index <<= 16;
    index >>= 16;
    result = sub_0822F0A4(index, GameState_GetBackupStateBase(), buffer);
    if (result != buffer + 8)
        return 0;
    return gUnk_03005404 = 1;
}
