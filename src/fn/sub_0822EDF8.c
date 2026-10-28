#include "global.h"
#include "ram.h"
#include "runtime.h"
#include "render.h"
struct Snapshot {
    u32 a, b;
    u16 c, d;
};
void MusicSlots_StopAllAndRetainResumable(void), InterruptsAndVideo_WaitForVBlank(void),
    m4aSoundVSyncOff(void), m4aSoundVSyncOn(void), Solar_PauseMeasurement(void);
u32 Solar_ResumeMeasurement(void);
u32 Save_ResumeRuntimeAfterTransfer(void)
{
    struct Snapshot *snap = &gSnapshot;
    *(volatile u16 *)0x04000202 = 1;
    gVideoTransferControl = 1;
    m4aSoundVSyncOn();
    InterruptsAndVideo_WaitForVBlank();
    *(volatile u16 *)0x04000208 = 0;
    gScanlineDmaSource = snap->a;
    gScanlineDmaTarget = snap->b;
    gScanlineDmaCount = snap->c;
    gScanlineDmaMode = snap->d;
    gClockSynced = gTransferSavedRuntimeFlag;
    *(volatile u16 *)0x04000208 = 1;
    return Solar_ResumeMeasurement();
}
