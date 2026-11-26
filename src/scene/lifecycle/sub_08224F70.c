#include "global.h"

void System_ShutdownAndSoftReset(void);

void SceneLifecycle_RequestRestart(u32 a)
{
    if (a == 0)
        *(u32 *)0x030053E8 = 0x140;
    else
        System_ShutdownAndSoftReset();
}
