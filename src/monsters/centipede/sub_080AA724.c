#include "global.h"
void TentacleActor_AdvanceMotionPhase(u8 *p, s32 f)
{
    if (f != 0) {
        u8 *q = *(u8 **)(p + 0x3d0);
        q[0x8d2] += 4;
    }
}
