#include "global.h"

void SceneLifecycle_InitFrameCounterPointer(void)
{
    *(u32 *)0x030053F8 = 0x030025C0;
}
