#include "global.h"
void Centipede_AdvanceMotionPhase(u8 *p, s32 f)
{
    if (f != 0) {
        (*(u8 **)(p + 0x3d0))[0x8d2] += 4;
    }
}
