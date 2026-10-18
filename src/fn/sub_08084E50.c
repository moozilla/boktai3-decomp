#include "global.h"

extern u8 gUnk_08605C64[];

void SceneActorControl_SetStoredMotionCallbacks(u8 *p)
{
    *(u8 **)(p + 0x29c) = gUnk_08605C64;
}
