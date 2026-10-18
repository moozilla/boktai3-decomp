#include "global.h"
struct Snapshot {
    u32 a, b;
    u16 c, d;
};
extern struct Snapshot gUnk_030054D0;
extern u32 gUnk_03004278, gUnk_03003A00, gUnk_03002608, gUnk_0300546C;
extern u16 gUnk_03004270, gUnk_03004274, gUnk_0300427C;
void sub_0822B410(void), InterruptsAndVideo_WaitForVBlank(void), m4aSoundVSyncOff(void), m4aSoundVSyncOn(void),
    Solar_PauseMeasurement(void), Solar_ResumeMeasurement(void);
u32 Save_SuspendRuntimeForTransfer(void)
{
    gUnk_030054D0.a = gUnk_03004278;
    gUnk_030054D0.b = gUnk_03003A00;
    gUnk_030054D0.c = gUnk_03004270;
    gUnk_030054D0.d = gUnk_03004274;
    gUnk_03004278 = 0;
    gUnk_03003A00 = 0;
    gUnk_03004270 = 0;
    gUnk_03004274 = 0;
    sub_0822B410();
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
