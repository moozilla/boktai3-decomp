#include "global.h"
#include "ram.h"
struct Snapshot {
    u32 a, b;
    u16 c, d;
};
extern u32 gUnk_03004278, gUnk_03003A00, gUnk_03002608, gUnk_0300546C;
extern u16 gUnk_03004270, gUnk_03004274, gUnk_0300427C;
void MusicSlots_StopAllAndRetainResumable(void), InterruptsAndVideo_WaitForVBlank(void),
    m4aSoundVSyncOff(void), m4aSoundVSyncOn(void), Solar_PauseMeasurement(void),
    Solar_ResumeMeasurement(void);
u32 Save_SuspendRuntimeForTransfer(void)
{
    gSnapshot.a = gUnk_03004278;
    gSnapshot.b = gUnk_03003A00;
    gSnapshot.c = gUnk_03004270;
    gSnapshot.d = gUnk_03004274;
    gUnk_03004278 = 0;
    gUnk_03003A00 = 0;
    gUnk_03004270 = 0;
    gUnk_03004274 = 0;
    MusicSlots_StopAllAndRetainResumable();
    InterruptsAndVideo_WaitForVBlank();
    gUnk_0300427C = 0;
    m4aSoundVSyncOff();
    InterruptsAndVideo_WaitForVBlank();
    *(volatile u16 *)0x04000208 = 0;
    gUnk_03002608 = gUnk_0300546C;
    gUnk_0300546C = 1;
    *(volatile u16 *)0x04000208 = 1;
    Solar_PauseMeasurement();
    return 0;
}
