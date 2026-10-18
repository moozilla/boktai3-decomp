#include "global.h"

u32 rfu_syncVBlank(void);
void RfuManager_NotifyEvent(u32, u32);
void RfuManager_CompletePendingMscCycle(void);

void RfuManager_CheckVBlankSync(void)
{
    if ((u16)rfu_syncVBlank() != 0) {
        RfuManager_NotifyEvent(0xf1, 0);
        RfuManager_CompletePendingMscCycle();
    }
}
