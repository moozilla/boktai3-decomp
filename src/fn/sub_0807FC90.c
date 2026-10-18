#include "global.h"

void SceneActorControl_OnIdle(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
}
