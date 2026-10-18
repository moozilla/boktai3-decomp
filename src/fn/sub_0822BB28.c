#include "global.h"
u8 *Save_GetContinuationData(void);
u32 GameState_GetBackupRegionPointer(u8 *);
u32 Save_GetContinuationSnapshot(void)
{
    return GameState_GetBackupRegionPointer(Save_GetContinuationData());
}
