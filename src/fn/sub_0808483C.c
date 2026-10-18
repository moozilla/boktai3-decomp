#include "global.h"

extern u8 gUnk_08605C5C[];

void SceneActorControl_SetApproachCallbacks(u8 *p)
{
    *(u8 **)(p + 0x28c) = gUnk_08605C5C;
}
