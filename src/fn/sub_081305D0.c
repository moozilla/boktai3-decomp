#include "global.h"
void SceneActorControl_AdvancePhase(u8 *p)
{
    u8 *q = p + 0x2b1;
    if (*q != 0)
        *q = 0;
    *(s32 *)(p + 0x2ac) += 1;
}
