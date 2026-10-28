#include "global.h"
#include "game_state.h"
#include "ram.h"
extern u32 gUnk_03005404;
void GameState_BackupRegionRange(void *, u32);
u32 Save_AlignSizeToEightBytes(u32);
u32 Save_GetPrimaryRegionBlock(u32, u32);
void *GameState_GetBackupVariableBase(void), *GameState_GetBackupStateBase(void);
s32 Save_WriteBlockWithRetries(u32, void *, u32);
u32 Save_GetMainStateSize(void);
u32 Save_WriteMainState(u32 arg)
{
    u32 buffer, result, index;
    *(u32 *)gGameState = gSaveStateSignature;
    GameState_BackupRegionRange(gGameState, 4);
    buffer = Save_AlignSizeToEightBytes(1024);
    index = Save_GetPrimaryRegionBlock(arg, 0);
    index <<= 16;
    index >>= 16;
    result = Save_WriteBlockWithRetries(index, GameState_GetBackupVariableBase(), buffer);
    if (result != buffer + 8)
        return 0;
    buffer = Save_AlignSizeToEightBytes(Save_GetMainStateSize());
    index = Save_GetPrimaryRegionBlock(arg, 1);
    index <<= 16;
    index >>= 16;
    result = Save_WriteBlockWithRetries(index, GameState_GetBackupStateBase(), buffer);
    if (result != buffer + 8)
        return 0;
    return gUnk_03005404 = 1;
}
