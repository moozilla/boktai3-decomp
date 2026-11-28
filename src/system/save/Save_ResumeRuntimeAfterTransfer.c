#include "global.h"
#include "engine/ram.h"
#include "engine/runtime.h"
#include "gba/io_reg.h"
#include "engine/video/render.h"
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
    REG_IF = 1;
    gVideoTransferControl = 1;
    m4aSoundVSyncOn();
    InterruptsAndVideo_WaitForVBlank();
    REG_IME = 0;
    gScanlineDmaSource = snap->a;
    gScanlineDmaTarget = snap->b;
    gScanlineDmaCount = snap->c;
    gScanlineDmaMode = snap->d;
    gClockSynced = gTransferSavedRuntimeFlag;
    REG_IME = 1;
    return Solar_ResumeMeasurement();
}
