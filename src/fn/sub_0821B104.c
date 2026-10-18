#include "global.h"
extern u8 *gUnk_02000710, *gUnk_0200070C, *gUnk_02000708, *gUnk_02000704, *gUnk_02000700;
void Memory_Clear(u8 *, u32);
void Memory_Copy(u8 *, u8 *, u32);
void GameState_BackupActiveRegions(void);
void GameState_ResetActiveRegions(void)
{
    u16 v = *(u16 *)(gUnk_02000710 + 0x744);
    Memory_Clear(gUnk_02000708, 0x400);
    Memory_Clear(gUnk_02000710 + 0x30, 0x74A);
    *(u16 *)(gUnk_02000710 + 0x744) = v;
    GameState_BackupActiveRegions();
}
