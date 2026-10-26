#include "global.h"
#include "ram.h"
struct Snapshot {
    u32 a, b;
    u16 c, d;
};
extern u32 gUnk_03004278, gUnk_03003A00, gUnk_03002608, gUnk_0300546C;
extern u16 gUnk_03004270, gUnk_03004274, gUnk_0300427C;
void MusicSlots_StopAllAndRetainResumable(void), InterruptsAndVideo_WaitForVBlank(void),
    m4aSoundVSyncOff(void), m4aSoundVSyncOn(void), Solar_PauseMeasurement(void);
u32 Solar_ResumeMeasurement(void);
u32 Save_ResumeRuntimeAfterTransfer(void)
{
    struct Snapshot *snap = &gSnapshot;
    *(volatile u16 *)0x04000202 = 1;
    gUnk_0300427C = 1;
    m4aSoundVSyncOn();
    InterruptsAndVideo_WaitForVBlank();
    *(volatile u16 *)0x04000208 = 0;
    gUnk_03004278 = snap->a;
    gUnk_03003A00 = snap->b;
    gUnk_03004270 = snap->c;
    gUnk_03004274 = snap->d;
    gUnk_0300546C = gUnk_03002608;
    *(volatile u16 *)0x04000208 = 1;
    return Solar_ResumeMeasurement();
}
