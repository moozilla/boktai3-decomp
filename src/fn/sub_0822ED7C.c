#include "global.h"
#include "ram.h"
#include "runtime.h"
#include "render.h"
struct Snapshot {
    u32 a, b;
    u16 c, d;
};
extern u32 gUnk_0300546C;
void MusicSlots_StopAllAndRetainResumable(void), InterruptsAndVideo_WaitForVBlank(void),
    m4aSoundVSyncOff(void), m4aSoundVSyncOn(void), Solar_PauseMeasurement(void),
    Solar_ResumeMeasurement(void);
u32 Save_SuspendRuntimeForTransfer(void)
{
    gSnapshot.a = gScanlineDmaSource;
    gSnapshot.b = gScanlineDmaTarget;
    gSnapshot.c = gScanlineDmaCount;
    gSnapshot.d = gScanlineDmaMode;
    gScanlineDmaSource = 0;
    gScanlineDmaTarget = 0;
    gScanlineDmaCount = 0;
    gScanlineDmaMode = 0;
    MusicSlots_StopAllAndRetainResumable();
    InterruptsAndVideo_WaitForVBlank();
    gVideoTransferControl = 0;
    m4aSoundVSyncOff();
    InterruptsAndVideo_WaitForVBlank();
    *(volatile u16 *)0x04000208 = 0;
    gTransferSavedRuntimeFlag = gUnk_0300546C;
    gUnk_0300546C = 1;
    *(volatile u16 *)0x04000208 = 1;
    Solar_PauseMeasurement();
    return 0;
}
