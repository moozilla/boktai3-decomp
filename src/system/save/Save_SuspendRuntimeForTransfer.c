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
    REG_IME = 0;
    gTransferSavedRuntimeFlag = gClockSynced;
    gClockSynced = 1;
    REG_IME = 1;
    Solar_PauseMeasurement();
    return 0;
}
