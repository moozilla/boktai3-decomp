#include "global.h"
u8 *Save_GetContinuationData(void);
s32 Save_GetContinuationSize(void);
void GameState_BackupRegionRange(u8 *, s32);
void Save_SnapshotContinuation(void)
{
    u8 *a = Save_GetContinuationData();
    GameState_BackupRegionRange(a, Save_GetContinuationSize());
}
